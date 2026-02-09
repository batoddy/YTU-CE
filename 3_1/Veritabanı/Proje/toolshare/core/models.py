"""
ToolShare Database Models
"""

from django.db import models
from django.contrib.auth.models import User
from django.core.validators import MinValueValidator, MaxValueValidator


class Category(models.Model):
    name = models.CharField(max_length=100, unique=True, verbose_name="Kategori Adı")
    description = models.TextField(blank=True, null=True, verbose_name="Açıklama")
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = 'categories'
        verbose_name = 'Kategori'
        verbose_name_plural = 'Kategoriler'

    def __str__(self):
        return self.name


class UserProfile(models.Model):
    ROLE_CHOICES = [
        ('admin', 'Admin'),
        ('user', 'Kullanıcı'),
    ]
    
    user = models.OneToOneField(User, on_delete=models.CASCADE, related_name='profile')
    role = models.CharField(max_length=10, choices=ROLE_CHOICES, default='user', verbose_name="Rol")
    phone = models.CharField(max_length=20, blank=True, null=True, verbose_name="Telefon")
    address = models.TextField(blank=True, null=True, verbose_name="Adres")
    trust_score = models.DecimalField(
        max_digits=3, 
        decimal_places=2, 
        default=5.00,
        validators=[MinValueValidator(0), MaxValueValidator(5)],
        verbose_name="Güvenilirlik Skoru"
    )
    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'user_profiles'
        verbose_name = 'Kullanıcı Profili'
        verbose_name_plural = 'Kullanıcı Profilleri'

    def __str__(self):
        return f"{self.user.username} ({self.role})"


class Tool(models.Model):
    STATUS_CHOICES = [
        ('available', 'Müsait'),
        ('reserved', 'Rezerve'),
        ('maintenance', 'Bakımda'),
    ]
    
    name = models.CharField(max_length=200, verbose_name="Alet Adı")
    description = models.TextField(blank=True, null=True, verbose_name="Açıklama")
    category = models.ForeignKey(
        Category, 
        on_delete=models.PROTECT, 
        related_name='tools',
        verbose_name="Kategori"
    )
    owner = models.ForeignKey(
        User, 
        on_delete=models.CASCADE, 
        related_name='owned_tools',
        verbose_name="Sahip"
    )
    status = models.CharField(max_length=20, choices=STATUS_CHOICES, default='available', verbose_name="Durum")
    daily_price = models.DecimalField(
        max_digits=10, 
        decimal_places=2, 
        default=0,
        validators=[MinValueValidator(0)],
        verbose_name="Günlük Ücret"
    )
    image_url = models.URLField(blank=True, null=True, verbose_name="Resim URL")
    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'tools'
        verbose_name = 'Alet'
        verbose_name_plural = 'Aletler'
        indexes = [
            models.Index(fields=['name'], name='idx_tool_name'),
            models.Index(fields=['status'], name='idx_tool_status'),
        ]

    def __str__(self):
        return f"{self.name} - {self.owner.username}"


class Reservation(models.Model):
    STATUS_CHOICES = [
        ('pending', 'Beklemede'),
        ('approved', 'Onaylandı'),
        ('rejected', 'Reddedildi'),
        ('completed', 'Tamamlandı'),
        ('cancelled', 'İptal Edildi'),
    ]
    
    tool = models.ForeignKey(
        Tool, 
        on_delete=models.CASCADE, 
        related_name='reservations',
        verbose_name="Alet"
    )
    borrower = models.ForeignKey(
        User, 
        on_delete=models.CASCADE, 
        related_name='reservations',
        verbose_name="Kiralayan"
    )
    start_date = models.DateField(verbose_name="Başlangıç Tarihi")
    end_date = models.DateField(verbose_name="Bitiş Tarihi")
    delivered_at = models.DateTimeField(blank=True, null=True, verbose_name="Teslim Tarihi")
    status = models.CharField(max_length=20, choices=STATUS_CHOICES, default='pending', verbose_name="Durum")
    total_price = models.DecimalField(max_digits=10, decimal_places=2, default=0, verbose_name="Toplam Ücret")
    notes = models.TextField(blank=True, null=True, verbose_name="Notlar")
    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'reservations'
        verbose_name = 'Rezervasyon'
        verbose_name_plural = 'Rezervasyonlar'

    def __str__(self):
        return f"{self.tool.name} - {self.borrower.username}"


class Rating(models.Model):
    reservation = models.OneToOneField(
        Reservation, 
        on_delete=models.CASCADE, 
        related_name='rating',
        verbose_name="Rezervasyon"
    )
    score = models.IntegerField(
        validators=[MinValueValidator(1), MaxValueValidator(5)],
        verbose_name="Puan"
    )
    comment = models.TextField(blank=True, null=True, verbose_name="Yorum")
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = 'ratings'
        verbose_name = 'Değerlendirme'
        verbose_name_plural = 'Değerlendirmeler'
        constraints = [
            models.CheckConstraint(
                condition=models.Q(score__gte=1) & models.Q(score__lte=5),
                name='rating_score_range'
            )
        ]

    def __str__(self):
        return f"{self.reservation.tool.name} - {self.score}/5"


class ToolRating(models.Model):
    reservation = models.OneToOneField(
        Reservation,
        on_delete=models.CASCADE,
        related_name='tool_rating',
        verbose_name="Rezervasyon"
    )
    tool = models.ForeignKey(
        Tool,
        on_delete=models.CASCADE,
        related_name='tool_ratings',
        verbose_name="Alet"
    )
    score = models.IntegerField(
        validators=[MinValueValidator(1), MaxValueValidator(5)],
        verbose_name="Puan"
    )
    comment = models.TextField(blank=True, null=True, verbose_name="Yorum")
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = 'tool_ratings'
        verbose_name = 'Alet Degerlendirme'
        verbose_name_plural = 'Alet Degerlendirmeler'
        constraints = [
            models.CheckConstraint(
                condition=models.Q(score__gte=1) & models.Q(score__lte=5),
                name='tool_rating_score_range'
            )
        ]

    def __str__(self):
        return f"{self.tool.name} - {self.score}/5"


class Notification(models.Model):
    user = models.ForeignKey(
        User,
        on_delete=models.CASCADE,
        related_name='notifications',
        verbose_name="Kullanici"
    )
    reservation = models.ForeignKey(
        Reservation,
        on_delete=models.CASCADE,
        related_name='notifications',
        blank=True,
        null=True,
        verbose_name="Rezervasyon"
    )
    message = models.CharField(max_length=255, verbose_name="Mesaj")
    is_read = models.BooleanField(default=False, verbose_name="Okundu")
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = 'notifications'
        verbose_name = 'Bildirim'
        verbose_name_plural = 'Bildirimler'

    def __str__(self):
        return f"{self.user.username} - {self.message}"


class AuditLog(models.Model):
    ACTION_CHOICES = [
        ('INSERT', 'Ekleme'),
        ('UPDATE', 'Güncelleme'),
        ('DELETE', 'Silme'),
    ]
    
    table_name = models.CharField(max_length=100, verbose_name="Tablo Adı")
    action = models.CharField(max_length=10, choices=ACTION_CHOICES, verbose_name="İşlem")
    record_id = models.IntegerField(verbose_name="Kayıt ID")
    old_values = models.JSONField(blank=True, null=True, verbose_name="Eski Değerler")
    new_values = models.JSONField(blank=True, null=True, verbose_name="Yeni Değerler")
    performed_by = models.CharField(max_length=100, blank=True, null=True, verbose_name="İşlemi Yapan")
    performed_at = models.DateTimeField(auto_now_add=True, verbose_name="İşlem Zamanı")
    message = models.TextField(blank=True, null=True, verbose_name="Bilgilendirme Mesajı")

    class Meta:
        db_table = 'audit_logs'
        verbose_name = 'Denetim Logu'
        verbose_name_plural = 'Denetim Logları'

    def __str__(self):
        return f"{self.table_name} - {self.action} - {self.performed_at}"
