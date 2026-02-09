"""
ToolShare Views
- CRUD işlemleri (INSERT, UPDATE, DELETE)
- Arama ve listeleme
- SQL fonksiyonları çağırma
- UNION/INTERSECT/EXCEPT sorguları
- Aggregate + HAVING sorguları
"""

from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.contrib import messages
from django.http import JsonResponse
from django.db import connection
from django.db.models import Count, Avg, Sum, Q, Value, DecimalField, Subquery, OuterRef
from django.db.models.functions import Coalesce
from django.views.decorators.http import require_http_methods
from django.utils import timezone
from .models import Category, UserProfile, Tool, Reservation, Rating, ToolRating, Notification, AuditLog
import json
from datetime import datetime, date, timedelta
import calendar
from decimal import Decimal


# =====================================================
# AUTHENTICATION VIEWS
# =====================================================

def login_view(request):
    if request.user.is_authenticated:
        return redirect('dashboard')
    
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')
        user = authenticate(request, username=username, password=password)
        
        if user is not None:
            login(request, user)
            messages.success(request, f'Hoş geldiniz, {user.first_name or user.username}!')
            return redirect('dashboard')
        else:
            messages.error(request, 'Kullanıcı adı veya şifre hatalı!')
    
    return render(request, 'login.html')


def logout_view(request):
    logout(request)
    messages.info(request, 'Başarıyla çıkış yaptınız.')
    return redirect('login')


def register_view(request):
    if request.method == 'POST':
        username = request.POST.get('username')
        email = request.POST.get('email')
        password = request.POST.get('password')
        password2 = request.POST.get('password2')
        first_name = request.POST.get('first_name')
        last_name = request.POST.get('last_name')
        phone = request.POST.get('phone')
        address = request.POST.get('address')
        
        if password != password2:
            messages.error(request, 'Şifreler eşleşmiyor!')
            return render(request, 'register.html')
        
        if User.objects.filter(username=username).exists():
            messages.error(request, 'Bu kullanıcı adı zaten kullanılıyor!')
            return render(request, 'register.html')
        
        user = User.objects.create_user(
            username=username,
            email=email,
            password=password,
            first_name=first_name,
            last_name=last_name
        )
        
        UserProfile.objects.create(
            user=user,
            role='user',
            phone=phone,
            address=address
        )
        
        messages.success(request, 'Kayıt başarılı! Giriş yapabilirsiniz.')
        return redirect('login')
    
    return render(request, 'register.html')


# =====================================================
# DASHBOARD
# =====================================================

@login_required
def dashboard(request):
    context = {
        'total_tools': Tool.objects.count(),
        'available_tools': Tool.objects.filter(status='available').count(),
        'total_reservations': Reservation.objects.count(),
        'pending_reservations': Reservation.objects.filter(status='pending').count(),
        'recent_tools': Tool.objects.select_related('category', 'owner').order_by('-created_at')[:5],
        'categories': Category.objects.annotate(tool_count=Count('tools')).order_by('name'),
    }
    return render(request, 'dashboard.html', context)


# =====================================================
# TOOL VIEWS (CRUD)
# =====================================================

@login_required
def tool_list(request):
    search_query = request.GET.get('search', '')
    category_id = request.GET.get('category', '')
    status = request.GET.get('status', '')
    
    tools = Tool.objects.select_related('category', 'owner')
    if 'tool_ratings' in connection.introspection.table_names():
        tools = tools.annotate(
            average_score=Coalesce(
                Avg('tool_ratings__score'),
                Value(0, output_field=DecimalField(max_digits=3, decimal_places=2)),
                output_field=DecimalField(max_digits=3, decimal_places=2),
            )
        )
    else:
        tools = tools.annotate(
            average_score=Value(0, output_field=DecimalField(max_digits=3, decimal_places=2))
        )
    
    if search_query:
        tools = tools.filter(name__icontains=search_query)
    if category_id:
        tools = tools.filter(category_id=category_id)
    if status:
        tools = tools.filter(status=status)
    
    categories = Category.objects.all()
    
    tools = list(tools)
    category_colors = {}
    color_palette = ['primary', 'success', 'warning', 'info', 'danger']
    for tool in tools:
        cat_id = tool.category_id
        if cat_id not in category_colors:
            category_colors[cat_id] = color_palette[len(category_colors) % len(color_palette)]
        tool.category_color = category_colors[cat_id]

    return render(request, 'tools/list.html', {
        'tools': tools,
        'categories': categories,
        'search_query': search_query,
        'selected_category': category_id,
        'selected_status': status,
    })


@login_required
def tool_detail(request, pk):
    tool = get_object_or_404(Tool, pk=pk)
    today = date.today()
    month_start = today.replace(day=1)
    def add_months(base_date, months):
        year = base_date.year + (base_date.month - 1 + months) // 12
        month = (base_date.month - 1 + months) % 12 + 1
        return date(year, month, 1)

    last_month = add_months(month_start, 11)
    last_month_end = date(
        last_month.year,
        last_month.month,
        calendar.monthrange(last_month.year, last_month.month)[1],
    )

    upcoming_reservations = Reservation.objects.filter(
        tool=tool,
        status__in=['pending', 'approved'],
        end_date__gte=today,
        start_date__lte=last_month_end,
    ).order_by('start_date')

    reserved_ranges = []
    reserved_dates = set()
    for res in upcoming_reservations:
        reserved_ranges.append({
            'start': res.start_date,
            'end': res.end_date,
            'status': res.status,
        })
        current = max(res.start_date, today)
        end_date = min(res.end_date, last_month_end)
        while current <= end_date:
            reserved_dates.add(current)
            current += timedelta(days=1)

    month_names = [
        'Ocak', 'Subat', 'Mart', 'Nisan', 'Mayis', 'Haziran',
        'Temmuz', 'Agustos', 'Eylul', 'Ekim', 'Kasim', 'Aralik'
    ]
    months = []
    cal = calendar.Calendar(firstweekday=0)
    for offset in range(0, 12):
        base = add_months(month_start, offset)
        year = base.year
        month = base.month
        weeks = []
        for week in cal.monthdatescalendar(year, month):
            week_days = []
            for day in week:
                in_month = day.month == month
                is_past = day < today
                is_reserved = day in reserved_dates
                is_disabled = (not in_month) or is_past or is_reserved or tool.status == 'maintenance'
                week_days.append({
                    'date': day,
                    'in_month': in_month,
                    'is_past': is_past,
                    'is_reserved': is_reserved,
                    'is_disabled': is_disabled,
                })
            weeks.append(week_days)
        months.append({
            'year': year,
            'month': month,
            'name': month_names[month - 1],
            'weeks': weeks,
        })

    next_reserved_range = reserved_ranges[0] if reserved_ranges else None
    context = {
        'tool': tool,
        'reserved_ranges': reserved_ranges,
        'months': months,
        'next_reserved_range': next_reserved_range,
        'today': today,
    }
    return render(request, 'tools/detail.html', context)


@login_required
def tool_comments(request, pk):
    tool = get_object_or_404(Tool, pk=pk)
    comments = ToolRating.objects.select_related('reservation', 'reservation__borrower').filter(
        tool=tool,
        comment__isnull=False,
    ).exclude(comment__exact='').order_by('-created_at')
    return render(request, 'tools/comments.html', {'tool': tool, 'comments': comments})


@login_required
def tool_create(request):
    if request.method == 'POST':
        tool = Tool.objects.create(
            name=request.POST.get('name'),
            description=request.POST.get('description'),
            category_id=request.POST.get('category'),
            owner=request.user,
            daily_price=request.POST.get('daily_price'),
            status='available'
        )
        messages.success(request, f'"{tool.name}" başarıyla eklendi!')
        return redirect('tool_list')
    
    return render(request, 'tools/create.html', {'categories': Category.objects.all()})


@login_required
def tool_update(request, pk):
    tool = get_object_or_404(Tool, pk=pk)
    
    if tool.owner != request.user and not request.user.is_staff:
        messages.error(request, 'Bu aleti düzenleme yetkiniz yok!')
        return redirect('tool_list')
    
    if request.method == 'POST':
        old_status = tool.status
        tool.name = request.POST.get('name')
        tool.description = request.POST.get('description')
        tool.category_id = request.POST.get('category')
        tool.daily_price = request.POST.get('daily_price')
        tool.status = request.POST.get('status')
        tool.save()
        
        if old_status != tool.status:
            messages.info(request, f'TRIGGER: Alet durumu "{old_status}" → "{tool.status}" olarak güncellendi!')
        
        messages.success(request, f'"{tool.name}" güncellendi!')
        return redirect('tool_list')
    
    return render(request, 'tools/update.html', {'tool': tool, 'categories': Category.objects.all()})


@login_required
def tool_delete(request, pk):
    tool = get_object_or_404(Tool, pk=pk)
    
    if tool.owner != request.user and not request.user.is_staff:
        messages.error(request, 'Bu aleti silme yetkiniz yok!')
        return redirect('tool_list')
    
    if request.method == 'POST':
        tool_name = tool.name
        try:
            tool.delete()
            messages.success(request, f'"{tool_name}" silindi!')
        except:
            messages.error(request, 'Silme hatası: Aktif rezervasyon olabilir.')
        return redirect('tool_list')
    
    return render(request, 'tools/delete.html', {'tool': tool})


# =====================================================
# RESERVATION VIEWS
# =====================================================

@login_required
def reservation_list(request):
    panel = request.GET.get('panel')
    unread_reservation_ids = []
    panel_notifications = []
    show_tool_rating = None
    show_owner_rating = None
    panel_counts = {
        'active': 0,
        'past': 0,
        'borrowed_active': 0,
        'borrowed_past': 0,
        'owned_active': 0,
        'owned_past': 0,
    }
    if request.user.is_authenticated:
        unread_qs = Notification.objects.filter(
            user=request.user,
            is_read=False,
            reservation__isnull=False,
        ).select_related('reservation', 'reservation__tool', 'reservation__borrower', 'reservation__tool__owner')

        def bucket_for(reservation):
            is_active = reservation.status in ['pending', 'approved']
            if request.user.is_staff:
                return 'active' if is_active else 'past'
            if reservation.borrower_id == request.user.id:
                return 'borrowed_active' if is_active else 'borrowed_past'
            if reservation.tool.owner_id == request.user.id:
                return 'owned_active' if is_active else 'owned_past'
            return None

        unread_list = list(unread_qs)
        for note in unread_list:
            bucket = bucket_for(note.reservation)
            if bucket:
                panel_counts[bucket] += 1

        if panel:
            for note in unread_list:
                bucket = bucket_for(note.reservation)
                if bucket == panel:
                    panel_notifications.append(note)
            if panel_notifications:
                Notification.objects.filter(id__in=[n.id for n in panel_notifications]).update(is_read=True)
                unread_reservation_ids = list(
                    {n.reservation_id for n in panel_notifications}
                )

        show_id = request.session.pop('show_tool_rating', None)
        if show_id:
            show_tool_rating = Reservation.objects.select_related('tool').filter(
                id=show_id,
                borrower=request.user,
                delivered_at__isnull=False,
            ).first()
            if show_tool_rating and hasattr(show_tool_rating, 'tool_rating'):
                show_tool_rating = None

        show_owner_id = request.session.pop('show_owner_rating', None)
        if show_owner_id:
            show_owner_rating = Reservation.objects.select_related('tool', 'borrower').filter(
                id=show_owner_id,
                tool__owner=request.user,
                status='completed',
            ).first()
            if show_owner_rating and hasattr(show_owner_rating, 'rating'):
                show_owner_rating = None

    borrower_trust_subquery = UserProfile.objects.filter(
        user_id=OuterRef('borrower_id')
    ).values('trust_score')[:1]
    borrower_trust_expr = Coalesce(
        Subquery(borrower_trust_subquery, output_field=DecimalField(max_digits=3, decimal_places=2)),
        Value(0, output_field=DecimalField(max_digits=3, decimal_places=2)),
        output_field=DecimalField(max_digits=3, decimal_places=2),
    )

    today = date.today()
    if request.user.is_staff:
        reservations = Reservation.objects.select_related('tool', 'borrower', 'tool__owner').annotate(
            borrower_trust_score=borrower_trust_expr
        )
        active_reservations = reservations.filter(
            status__in=['pending', 'approved'],
        )
        past_reservations = reservations.exclude(
            status__in=['pending', 'approved']
        )
        context = {
            'active_reservations': active_reservations.order_by('-created_at'),
            'past_reservations': past_reservations.order_by('-created_at'),
            'unread_reservation_ids': unread_reservation_ids,
            'panel_counts': panel_counts,
            'panel_notifications': panel_notifications,
            'panel': panel,
            'show_tool_rating': show_tool_rating,
            'show_owner_rating': show_owner_rating,
        }
    else:
        borrowed_reservations = Reservation.objects.select_related(
            'tool', 'borrower', 'tool__owner'
        ).filter(borrower=request.user).annotate(
            borrower_trust_score=borrower_trust_expr
        )
        owned_tool_reservations = Reservation.objects.select_related(
            'tool', 'borrower', 'tool__owner'
        ).filter(tool__owner=request.user).annotate(
            borrower_trust_score=borrower_trust_expr
        )
        active_borrowed = borrowed_reservations.filter(
            status__in=['pending', 'approved'],
        )
        past_borrowed = borrowed_reservations.exclude(
            status__in=['pending', 'approved']
        )
        active_owned = owned_tool_reservations.filter(
            status__in=['pending', 'approved'],
        )
        past_owned = owned_tool_reservations.exclude(
            status__in=['pending', 'approved']
        )
        context = {
            'active_borrowed_reservations': active_borrowed.order_by('-created_at'),
            'past_borrowed_reservations': past_borrowed.order_by('-created_at'),
            'active_owned_tool_reservations': active_owned.order_by('-created_at'),
            'past_owned_tool_reservations': past_owned.order_by('-created_at'),
            'unread_reservation_ids': unread_reservation_ids,
            'panel_counts': panel_counts,
            'panel_notifications': panel_notifications,
            'panel': panel,
            'show_tool_rating': show_tool_rating,
            'show_owner_rating': show_owner_rating,
        }

    return render(request, 'reservations/list.html', context)


@login_required
def borrower_history(request, user_id):
    borrower = get_object_or_404(User, pk=user_id)
    if not request.user.is_staff:
        has_relation = Reservation.objects.filter(
            tool__owner=request.user,
            borrower=borrower,
        ).exists()
        if not has_relation:
            messages.error(request, 'Bu kullanicinin gecmisine erisim yetkiniz yok!')
            return redirect('reservation_list')

    ratings_given = ToolRating.objects.select_related('tool').filter(
        reservation__borrower=borrower
    ).order_by('-created_at')
    owner_comments = Rating.objects.select_related('reservation', 'reservation__tool', 'reservation__tool__owner').filter(
        reservation__borrower=borrower
    ).order_by('-created_at')

    context = {
        'borrower': borrower,
        'ratings_given': ratings_given,
        'owner_comments': owner_comments,
    }
    return render(request, 'borrowers/history.html', context)


@login_required
def reservation_create(request, tool_id):
    tool = get_object_or_404(Tool, pk=tool_id)
    
    if tool.owner == request.user:
        messages.error(request, 'Kendi aletinizi rezerve edemezsiniz!')
        return redirect('tool_list')

    if tool.status == 'maintenance':
        messages.error(request, 'Bu alet su anda bakimda!')
        return redirect('tool_list')
    
    if request.method == 'POST':
        start_date = request.POST.get('start_date')
        end_date = request.POST.get('end_date')
        notes = request.POST.get('notes')
        
        start = datetime.strptime(start_date, '%Y-%m-%d').date()
        end = datetime.strptime(end_date, '%Y-%m-%d').date()
        today = date.today()
        if start < today or end < today:
            messages.error(request, 'Gecmise rezervasyon yapilamaz!')
            return render(request, 'reservations/create.html', {'tool': tool})
        if end < start:
            messages.error(request, 'Bitis tarihi baslangictan once olamaz!')
            return render(request, 'reservations/create.html', {'tool': tool})
        days = (end - start).days + 1
        total_price = days * tool.daily_price
        
        with connection.cursor() as cursor:
            cursor.execute(
                "SELECT * FROM get_tool_availability(%s, %s, %s)",
                [tool_id, start_date, end_date]
            )
            result = cursor.fetchone()
            
            if result and not result[0]:
                messages.error(request, result[2])
                return render(request, 'reservations/create.html', {'tool': tool})
        
        reservation = Reservation.objects.create(
            tool=tool,
            borrower=request.user,
            start_date=start,
            end_date=end,
            total_price=total_price,
            notes=notes,
            status='pending'
        )
        
        Notification.objects.create(
            user=tool.owner,
            reservation=reservation,
            message=f"Yeni rezervasyon istegi: {tool.name} ({request.user.username})"
        )
        
        last_log = AuditLog.objects.filter(table_name='reservations', record_id=reservation.id).last()
        if last_log:
            messages.info(request, last_log.message)
        
        messages.success(request, f'Rezervasyon oluşturuldu! Toplam: {total_price} TL')
        return redirect('reservation_list')
    
    return render(request, 'reservations/create.html', {'tool': tool, 'today': date.today()})


@login_required
def reservation_update_status(request, pk):
    reservation = get_object_or_404(Reservation, pk=pk)
    
    if reservation.tool.owner != request.user and not request.user.is_staff:
        messages.error(request, 'Bu işlem için yetkiniz yok!')
        return redirect('reservation_list')
    
    if request.method == 'POST':
        new_status = request.POST.get('status')
        reservation.status = new_status
        reservation.save()
        
        if new_status == 'approved':
            reservation.tool.status = 'reserved'
            reservation.tool.save()
        elif new_status in ['completed', 'cancelled', 'rejected']:
            reservation.tool.status = 'available'
            reservation.tool.save()
            if new_status == 'completed':
                request.session['show_owner_rating'] = reservation.id
        
        status_label = reservation.get_status_display()
        Notification.objects.create(
            user=reservation.borrower,
            reservation=reservation,
            message=f"Rezervasyon durumu guncellendi: {status_label}"
        )
        
        messages.success(request, f'Rezervasyon durumu: {new_status}')
        return redirect('reservation_list')
    
    return render(request, 'reservations/update_status.html', {'reservation': reservation})


@login_required
@require_http_methods(["POST"])
def reservation_mark_delivered(request, pk):
    reservation = get_object_or_404(Reservation, pk=pk)
    
    if reservation.borrower != request.user:
        messages.error(request, 'Bu iYlem iÇõin yetkiniz yok!')
        return redirect('reservation_list')
    
    if reservation.status != 'approved':
        messages.error(request, 'Sadece onaylanmis rezervasyonlar teslim edilebilir!')
        return redirect('reservation_list')
    
    if reservation.delivered_at:
        messages.info(request, 'Bu rezervasyon zaten teslim edildi.')
        return redirect('reservation_list')
    
    reservation.delivered_at = timezone.now()
    reservation.save(update_fields=['delivered_at'])
    request.session['show_tool_rating'] = reservation.id
    Notification.objects.create(
        user=reservation.tool.owner,
        reservation=reservation,
        message=f"Teslim bildirimi: {reservation.tool.name}"
    )
    messages.success(request, 'Teslim tarihi kaydedildi.')
    return redirect('reservation_list')


# =====================================================
# RATING VIEWS
# =====================================================

@login_required
def rating_create(request, reservation_id):
    reservation = get_object_or_404(Reservation, pk=reservation_id)
    
    if reservation.tool.owner != request.user and not request.user.is_staff:
        messages.error(request, 'Sadece kendi aletinizin rezervasyonlarini degerlendirebilirsiniz!')
        return redirect('reservation_list')
    
    if reservation.status != 'completed':
        messages.error(request, 'Sadece tamamlanmış rezervasyonlar değerlendirilebilir!')
        return redirect('reservation_list')
    
    if hasattr(reservation, 'rating'):
        messages.error(request, 'Bu rezervasyon zaten değerlendirilmiş!')
        return redirect('reservation_list')
    
    if reservation.delivered_at is None:
        messages.error(request, 'Teslim edilmeden degerlendirme yapilamaz!')
        return redirect('reservation_list')
    
    if request.method == 'POST':
        raw_score = int(request.POST.get('score'))
        score = raw_score
        delivered_date = timezone.localtime(reservation.delivered_at).date()
        if delivered_date > reservation.end_date and raw_score == 5:
            score = 3

        Rating.objects.create(
            reservation=reservation,
            score=score,
            comment=request.POST.get('comment')
        )
        Notification.objects.create(
            user=reservation.borrower,
            reservation=reservation,
            message=f"Owner sizi puanladi: {reservation.tool.name}"
        )
        if score != raw_score:
            messages.info(request, 'Gec teslim nedeniyle puan 3 olarak kaydedildi.')
        messages.success(request, 'Degerlendirme kaydedildi!')
        return redirect('reservation_list')
    
    return render(request, 'ratings/create.html', {'reservation': reservation})


@login_required
def tool_rating_create(request, reservation_id):
    reservation = get_object_or_404(Reservation, pk=reservation_id)
    
    if reservation.borrower != request.user:
        messages.error(request, 'Sadece kendi rezervasyonlariniz icin alet puanlayabilirsiniz!')
        return redirect('reservation_list')
    
    if reservation.delivered_at is None or reservation.status not in ['approved', 'completed']:
        messages.error(request, 'Teslim edilmeden alet puanlanamaz!')
        return redirect('reservation_list')
    
    if hasattr(reservation, 'tool_rating'):
        messages.error(request, 'Bu alet zaten puanlanmis!')
        return redirect('reservation_list')
    
    if request.method == 'POST':
        ToolRating.objects.create(
            reservation=reservation,
            tool=reservation.tool,
            score=int(request.POST.get('score')),
            comment=request.POST.get('comment')
        )
        Notification.objects.create(
            user=reservation.tool.owner,
            reservation=reservation,
            message=f"Alet puanlandi: {reservation.tool.name}"
        )
        messages.success(request, 'Alet puani kaydedildi!')
        return redirect('reservation_list')
    
    return render(request, 'ratings/tool_create.html', {'reservation': reservation})


# =====================================================
# CATEGORY VIEWS
# =====================================================

@login_required
def category_list(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    categories = Category.objects.annotate(tool_count=Count('tools'))
    return render(request, 'categories/list.html', {'categories': categories})


@login_required
def category_create(request):
    if not request.user.is_staff:
        messages.error(request, 'Kategori eklemek için admin olmalısınız!')
        return redirect('category_list')
    
    if request.method == 'POST':
        Category.objects.create(
            name=request.POST.get('name'),
            description=request.POST.get('description')
        )
        messages.success(request, 'Kategori eklendi!')
        return redirect('category_list')
    
    return render(request, 'categories/create.html')


@login_required
def category_update(request, pk):
    if not request.user.is_staff:
        messages.error(request, 'Bu işlem için admin olmalısınız!')
        return redirect('category_list')
    
    category = get_object_or_404(Category, pk=pk)
    
    if request.method == 'POST':
        category.name = request.POST.get('name')
        category.description = request.POST.get('description')
        category.save()
        messages.success(request, 'Kategori güncellendi!')
        return redirect('category_list')
    
    return render(request, 'categories/update.html', {'category': category})


@login_required
def category_delete(request, pk):
    if not request.user.is_staff:
        messages.error(request, 'Bu işlem için admin olmalısınız!')
        return redirect('category_list')
    
    category = get_object_or_404(Category, pk=pk)
    
    if request.method == 'POST':
        try:
            category.delete()
            messages.success(request, 'Kategori silindi!')
        except:
            messages.error(request, 'SİLME KISITI: Bu kategoriye ait aletler var!')
        return redirect('category_list')
    
    return render(request, 'categories/delete.html', {'category': category})


# =====================================================
# REPORTS - UNION/INTERSECT/EXCEPT + AGGREGATE
# =====================================================

@login_required
def reports(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    return render(request, 'reports/index.html')


@login_required
def report_union(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM v_tools_union ORDER BY name")
        results = cursor.fetchall()
    return render(request, 'reports/union.html', {'results': results})


@login_required
def report_intersect(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM v_users_intersect")
        results = cursor.fetchall()
    return render(request, 'reports/intersect.html', {'results': results})


@login_required
def report_except(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM v_users_except")
        results = cursor.fetchall()
    return render(request, 'reports/except.html', {'results': results})


@login_required
def report_aggregate(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    with connection.cursor() as cursor:
        cursor.execute("SELECT kategori, alet_sayisi, ortalama_fiyat, toplam_fiyat, max_fiyat, min_fiyat FROM v_category_stats ORDER BY alet_sayisi DESC")
        results = cursor.fetchall()
    return render(request, 'reports/aggregate.html', {'results': results})


# =====================================================
# SQL FUNCTIONS
# =====================================================

@login_required
def sql_function_availability(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    result = None
    if request.method == 'POST':
        with connection.cursor() as cursor:
            cursor.execute(
                "SELECT * FROM get_tool_availability(%s, %s, %s)",
                [request.POST.get('tool_id'), request.POST.get('start_date'), request.POST.get('end_date')]
            )
            row = cursor.fetchone()
            if row:
                result = {'is_available': row[0], 'conflicting_reservations': row[1], 'message': row[2]}
    
    return render(request, 'reports/func_availability.html', {'result': result, 'tools': Tool.objects.all()})


@login_required
def sql_function_trust_score(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    result = None
    if request.method == 'POST':
        with connection.cursor() as cursor:
            cursor.execute("SELECT * FROM calculate_user_trust_score(%s)", [request.POST.get('user_id')])
            row = cursor.fetchone()
            if row:
                result = {
                    'user_id': row[0], 'username': row[1], 'completed_rentals': row[2],
                    'average_rating': row[3], 'on_time_returns': row[4],
                    'calculated_trust_score': row[5], 'trust_level': row[6]
                }
    
    return render(request, 'reports/func_trust_score.html', {'result': result, 'users': User.objects.all()})


@login_required
def sql_function_category_stats(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM get_category_statistics()")
        results = cursor.fetchall()
    return render(request, 'reports/func_category_stats.html', {'results': results})


@login_required
def view_tool_details(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    with connection.cursor() as cursor:
        cursor.execute("SELECT * FROM v_tool_details ORDER BY tool_name")
        columns = [col[0] for col in cursor.description]
        results = [dict(zip(columns, row)) for row in cursor.fetchall()]
    return render(request, 'reports/view_tool_details.html', {'results': results})


# =====================================================
# ADMIN PANEL
# =====================================================

@login_required
def admin_panel(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    
    return render(request, 'admin_panel.html', {
        'users': User.objects.select_related('profile').all(),
        'categories': Category.objects.annotate(tool_count=Count('tools')),
        'audit_logs': AuditLog.objects.order_by('-performed_at')[:20],
    })


@login_required
def admin_user_role(request, user_id):
    if not request.user.is_staff:
        messages.error(request, 'Bu işlem için yetkiniz yok!')
        return redirect('dashboard')
    
    user = get_object_or_404(User, pk=user_id)
    profile, _ = UserProfile.objects.get_or_create(user=user)
    
    if request.method == 'POST':
        new_role = request.POST.get('role')
        profile.role = new_role
        profile.save()
        user.is_staff = (new_role == 'admin')
        user.save()
        messages.success(request, f'{user.username} rolü: {new_role}')
        return redirect('admin_panel')
    
    return render(request, 'admin_user_role.html', {'target_user': user, 'profile': profile})


@login_required
def audit_logs(request):
    if not request.user.is_staff:
        messages.error(request, 'Bu sayfaya erişim yetkiniz yok!')
        return redirect('dashboard')
    return render(request, 'audit_logs.html', {'logs': AuditLog.objects.order_by('-performed_at')[:50]})
