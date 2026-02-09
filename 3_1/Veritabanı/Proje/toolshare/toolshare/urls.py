from django.contrib import admin
from django.urls import path
from core import views

urlpatterns = [
    path('admin/', admin.site.urls),
    
    # Auth
    path('', views.login_view, name='login'),
    path('login/', views.login_view, name='login'),
    path('logout/', views.logout_view, name='logout'),
    path('register/', views.register_view, name='register'),
    
    # Dashboard
    path('dashboard/', views.dashboard, name='dashboard'),
    
    # Tools (CRUD)
    path('tools/', views.tool_list, name='tool_list'),
    path('tools/<int:pk>/', views.tool_detail, name='tool_detail'),
    path('tools/<int:pk>/comments/', views.tool_comments, name='tool_comments'),
    path('tools/create/', views.tool_create, name='tool_create'),
    path('tools/<int:pk>/update/', views.tool_update, name='tool_update'),
    path('tools/<int:pk>/delete/', views.tool_delete, name='tool_delete'),
    
    # Reservations
    path('reservations/', views.reservation_list, name='reservation_list'),
    path('reservations/create/<int:tool_id>/', views.reservation_create, name='reservation_create'),
    path('reservations/<int:pk>/status/', views.reservation_update_status, name='reservation_update_status'),
    path('reservations/<int:pk>/delivered/', views.reservation_mark_delivered, name='reservation_mark_delivered'),

    # Borrower history
    path('borrowers/<int:user_id>/history/', views.borrower_history, name='borrower_history'),
    
    # Ratings
    path('ratings/create/<int:reservation_id>/', views.rating_create, name='rating_create'),
    path('ratings/tools/create/<int:reservation_id>/', views.tool_rating_create, name='tool_rating_create'),
    
    # Categories
    path('categories/', views.category_list, name='category_list'),
    path('categories/create/', views.category_create, name='category_create'),
    path('categories/<int:pk>/update/', views.category_update, name='category_update'),
    path('categories/<int:pk>/delete/', views.category_delete, name='category_delete'),
    
    # Reports
    path('reports/', views.reports, name='reports'),
    path('reports/union/', views.report_union, name='report_union'),
    path('reports/intersect/', views.report_intersect, name='report_intersect'),
    path('reports/except/', views.report_except, name='report_except'),
    path('reports/aggregate/', views.report_aggregate, name='report_aggregate'),
    
    # SQL Functions
    path('reports/func/availability/', views.sql_function_availability, name='sql_function_availability'),
    path('reports/func/trust-score/', views.sql_function_trust_score, name='sql_function_trust_score'),
    path('reports/func/category-stats/', views.sql_function_category_stats, name='sql_function_category_stats'),
    path('reports/view/tool-details/', views.view_tool_details, name='view_tool_details'),
    
    # Admin Panel
    path('admin-panel/', views.admin_panel, name='admin_panel'),
    path('admin-panel/user/<int:user_id>/role/', views.admin_user_role, name='admin_user_role'),
    path('admin-panel/logs/', views.audit_logs, name='audit_logs'),
]
