from django.utils import timezone
from datetime import date
from decimal import Decimal
from .models import Notification, Reservation, UserProfile


def check_overdue_reservations(user):
    """
    Süresi geçmiş ve teslim edilmemiş rezervasyonları kontrol et.
    - Borrower'a hatırlatma bildirimi gönder (skor düşürme bilgisiyle)
    - Alet sahibine bildirim gönder
    - Güven skorunu düşür (-0.1/gün, minimum 0)
    """
    today = date.today()
    
    # Kullanıcının süresi geçmiş, teslim edilmemiş, onaylanmış rezervasyonları
    overdue_reservations = Reservation.objects.filter(
        borrower=user,
        status='approved',
        end_date__lt=today,
        delivered_at__isnull=True
    ).select_related('tool', 'tool__owner')
    
    for reservation in overdue_reservations:
        # Kaç gün geçmiş?
        days_overdue = (today - reservation.end_date).days
        
        # Bu rezervasyon için bugün bildirim gönderilmiş mi?
        today_start = timezone.now().replace(hour=0, minute=0, second=0, microsecond=0)
        already_notified_today = Notification.objects.filter(
            user=user,
            reservation=reservation,
            message__icontains='gecikti',
            created_at__gte=today_start
        ).exists()
        
        if not already_notified_today:
            # Güven skorunu düşür (-0.1)
            score_reduced = False
            old_score = Decimal('5.00')
            new_score = Decimal('5.00')
            
            try:
                profile = UserProfile.objects.get(user=user)
                old_score = profile.trust_score
                new_score = max(Decimal('0.00'), profile.trust_score - Decimal('0.10'))
                if new_score != profile.trust_score:
                    profile.trust_score = new_score
                    profile.save(update_fields=['trust_score', 'updated_at'])
                    score_reduced = True
            except UserProfile.DoesNotExist:
                pass
            
            # Borrower'a hatırlatma bildirimi (gecikme + skor düşürme bilgisi)
            if score_reduced:
                borrower_message = f"⚠️ '{reservation.tool.name}' iadesi {days_overdue} gün gecikti! Güven skorunuz düşürüldü: {old_score:.2f} → {new_score:.2f}. Lütfen hemen teslim edin!"
            else:
                borrower_message = f"⚠️ '{reservation.tool.name}' iadesi {days_overdue} gün gecikti! Lütfen hemen teslim edin!"
            
            Notification.objects.create(
                user=user,
                reservation=reservation,
                message=borrower_message
            )
            
            # Alet sahibine de bildirim
            Notification.objects.create(
                user=reservation.tool.owner,
                reservation=reservation,
                message=f"⚠️ '{reservation.tool.name}' iadesi {days_overdue} gün gecikti! Kiralayan: {user.username}"
            )


def notifications(request):
    """
    Her sayfa yüklemesinde:
    1. Geç iade kontrolü yap
    2. Okunmamış bildirim sayısını döndür
    """
    unread_count = 0
    
    if request.user.is_authenticated:
        # Geç iade kontrolü yap
        check_overdue_reservations(request.user)
        
        # Okunmamış bildirim sayısı
        unread_count = Notification.objects.filter(
            user=request.user, 
            is_read=False
        ).count()
    
    return {'notification_unread_count': unread_count}