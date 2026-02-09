# ToolShare - Mahalle Alet Paylaşım Sistemi

Veritabanı Lab 2526 Projesi - Django + PostgreSQL

## Ödev Gereksinimleri Karşılama Tablosu

| # | Gereksinim | Durum | Açıklama |
|---|------------|-------|----------|
| 1 | En az 4 tablo, 10'ar kayıt | ✅ | 6 tablo: categories, user_profiles, tools, reservations, ratings, audit_logs |
| 2 | Primary Key ve Foreign Key | ✅ | Tüm tablolarda PK, ilişkili tablolarda FK mevcut |
| 3 | Silme kısıtı ve sayı kısıtı | ✅ | Category PROTECT, Rating score CHECK(1-5) |
| 4 | INSERT, UPDATE, DELETE | ✅ | Tüm CRUD işlemleri arayüzde mevcut |
| 5 | Değere göre arama/listeleme | ✅ | Alet arama (INDEX kullanır) |
| 6 | VIEW kullanımı | ✅ | v_tool_details, v_user_statistics, v_active_reservations |
| 7 | INDEX oluşturma | ✅ | idx_tool_name, idx_tool_status |
| 8 | SEQUENCE kullanımı | ✅ | Reservation ID otomatik artış |
| 9 | UNION/INTERSECT/EXCEPT | ✅ | Raporlar sayfasında üçü de mevcut |
| 10 | Aggregate + HAVING | ✅ | Kategori istatistikleri (COUNT, AVG, SUM, MAX, MIN + HAVING) |
| 11 | 3 SQL Fonksiyonu | ✅ | get_tool_availability, calculate_user_trust_score (RECORD), get_category_statistics (CURSOR) |
| 12 | 2 Trigger | ✅ | trg_reservation_insert, trg_tool_status_update (bilgilendirme mesajı döndürür) |
| 13 | Admin/User rolleri | ✅ | 2 farklı rol: admin ve user |

## Kurulum

### 1. PostgreSQL Veritabanı Oluştur

```sql
-- pgAdmin veya psql'de çalıştır:
CREATE DATABASE toolshare_db;


```

### 2. Python Bağımlılıklarını Kur

```bash
cd toolshare
pip install -r requirements.txt
```

### 3. Veritabanı Ayarlarını Düzenle

`toolshare/settings.py` dosyasında PostgreSQL bilgilerinizi girin:

```python
DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.postgresql',
        'NAME': 'toolshare_db',      #  oluşturduğumuz DB adı
        'USER': 'toolshare_user',    #  oluşturduğumuz Kullanıcı
        'PASSWORD': '12345',         # Belirlediğimiz şifre
        'HOST': 'localhost',
        'PORT': '5432',
    }
}
```

### 4. Migrations Oluştur ve Uygula

```bash
python manage.py makemigrations core
python manage.py migrate
python manage.py hash_plain_passwords


```

### 5. SQL Fonksiyonları, Trigger ve View'ları Oluştur

pgAdmin'de veya psql'de sırasıyla çalıştır:

```bash
# Sırasıyla bu dosyaları çalıştır:
psql -U postgres -d toolshare_db -f core/sql/init_db.sql
psql -U postgres -d toolshare_db -f core/sql/seed_data.sql
psql -U postgres -d toolshare_db -f core/sql/views.sql
psql -U postgres -d toolshare_db -f core/sql/functions.sql
psql -U postgres -d toolshare_db -f core/sql/triggers.sql
```

Veya pgAdmin'de Query Tool açıp dosya içeriklerini yapıştırın.

### 6. Örnek Verileri Yükle (Opsiyonel)

```bash
psql -U postgres -d toolshare_db -f core/sql/seed_data.sql
```

### 7. Admin Kullanıcı Oluştur

```bash
python manage.py createsuperuser
# Kullanıcı adı: admin
# E-posta: admin@test.com
# Şifre: admin123
```

### 8. Sunucuyu Başlat

```bash
python manage.py runserver
```

Tarayıcıda: http://127.0.0.1:8000

## Kullanım

### Giriş Bilgileri
- **Admin:** admin / admin123 (oluşturduğunuz)
- **User:** Kayıt ol sayfasından yeni kullanıcı oluşturabilirsiniz

### Önemli Sayfalar
- `/dashboard/` - Ana panel
- `/tools/` - Alet listesi (INSERT, UPDATE, DELETE, SEARCH)
- `/reservations/` - Rezervasyonlar
- `/categories/` - Kategoriler
- `/reports/` - SQL Raporları (UNION, INTERSECT, EXCEPT, Aggregate, Fonksiyonlar, VIEW)
- `/admin-panel/` - Admin paneli (rol yönetimi, audit logları)

## Proje Yapısı

```
toolshare/
├── manage.py
├── requirements.txt
├── README.md
├── er_diagram.mermaid
├── toolshare/
│   ├── settings.py
│   ├── urls.py
│   └── wsgi.py
├── core/
│   ├── models.py          # 6 tablo tanımı
│   ├── views.py           # Tüm endpoint'ler
│   ├── admin.py
│   └── sql/

│       ├── views.sql      # VIEW tanımları
│       ├── functions.sql  # 3 SQL fonksiyonu
│       ├── triggers.sql   # 2 trigger
│       └── seed_data.sql  # Örnek veriler
├── templates/
│   ├── base.html
│   ├── login.html
│   ├── dashboard.html
│   ├── tools/
│   ├── reservations/
│   ├── categories/
│   └── reports/
└── static/
    ├── css/
    └── js/
```

## Teknolojiler

- **Backend:** Django 4.2
- **Database:** PostgreSQL
- **Frontend:** Bootstrap 5, HTML5, JavaScript
- **Icons:** Bootstrap Icons

## Notlar

- Trigger'lar çalıştığında `audit_logs` tablosuna kayıt eklenir
- Admin panelinde trigger mesajlarını görebilirsiniz
- Kategori silmeye çalıştığınızda PROTECT kısıtı çalışır (kategoriye ait alet varsa silemezsiniz)
- Rating score 1-5 arası CHECK constraint ile sınırlıdır
