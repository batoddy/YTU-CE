from django.contrib import admin
from .models import Category, UserProfile, Tool, Reservation, Rating, AuditLog

@admin.register(Category)
class CategoryAdmin(admin.ModelAdmin):
    list_display = ['id', 'name', 'created_at']

@admin.register(UserProfile)
class UserProfileAdmin(admin.ModelAdmin):
    list_display = ['user', 'role', 'trust_score']

@admin.register(Tool)
class ToolAdmin(admin.ModelAdmin):
    list_display = ['id', 'name', 'category', 'owner', 'status', 'daily_price']

@admin.register(Reservation)
class ReservationAdmin(admin.ModelAdmin):
    list_display = ['id', 'tool', 'borrower', 'status', 'total_price']

@admin.register(Rating)
class RatingAdmin(admin.ModelAdmin):
    list_display = ['id', 'reservation', 'score']

@admin.register(AuditLog)
class AuditLogAdmin(admin.ModelAdmin):
    list_display = ['id', 'table_name', 'action', 'performed_at']
