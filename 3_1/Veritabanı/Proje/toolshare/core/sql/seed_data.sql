-- =====================================================
-- ÖRNEK VERİLER
-- Gereksinim 1: Her tabloda en az 10 kayıt bulunmalıdır
-- =====================================================

-- =====================================================
-- 1. TABLOLARI OLUŞTURMA (Önce bunları çalıştırın)
-- =====================================================

-- Categories Tablosu
CREATE TABLE IF NOT EXISTS categories (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT NOW()
);


-- User Profiles Tablosu
CREATE TABLE IF NOT EXISTS user_profiles (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES auth_user(id) ON DELETE CASCADE,
    role VARCHAR(50),
    phone VARCHAR(20),
    address TEXT,
    trust_score DECIMAL(3, 2) DEFAULT 0.00,
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Tools Tablosu
CREATE TABLE IF NOT EXISTS tools (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    category_id INTEGER REFERENCES categories(id) ON DELETE SET NULL,
    owner_id INTEGER REFERENCES auth_user(id) ON DELETE CASCADE,
    status VARCHAR(50) DEFAULT 'available',
    daily_price DECIMAL(10, 2),
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Reservations Tablosu
CREATE TABLE IF NOT EXISTS reservations (
    id SERIAL PRIMARY KEY,
    tool_id INTEGER REFERENCES tools(id) ON DELETE CASCADE,
    borrower_id INTEGER REFERENCES auth_user(id) ON DELETE CASCADE,
    start_date DATE NOT NULL,
    end_date DATE NOT NULL,
    status VARCHAR(50),
    total_price DECIMAL(10, 2),
    notes TEXT,
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Ratings Tablosu
CREATE TABLE IF NOT EXISTS ratings (
    id SERIAL PRIMARY KEY,
    reservation_id INTEGER REFERENCES reservations(id) ON DELETE CASCADE,
    score INTEGER CHECK (score >= 1 AND score <= 5),
    comment TEXT,
    created_at TIMESTAMP DEFAULT NOW()
);



-- Kategoriler (10 kayıt)
INSERT INTO categories (id, name, description, created_at) VALUES
(1, 'Elektrikli Aletler', 'Matkap, testere, taşlama gibi elektrikli el aletleri', NOW()),
(2, 'Bahçe Aletleri', 'Çim biçme, budama, sulama ekipmanları', NOW()),
(3, 'El Aletleri', 'Çekiç, tornavida, pense gibi manuel aletler', NOW()),
(4, 'Boyama Ekipmanları', 'Boya tabancası, rulo, fırça setleri', NOW()),
(5, 'Temizlik Ekipmanları', 'Basınçlı yıkama, süpürge makineleri', NOW()),
(6, 'Ölçüm Aletleri', 'Metre, su terazisi, lazer ölçüm cihazları', NOW()),
(7, 'Merdiven ve İskele', 'Katlanır merdiven, alüminyum iskele', NOW()),
(8, 'Kaynak Ekipmanları', 'Kaynak makinesi, kaynak maskesi', NOW()),
(9, 'Hırdavat', 'Vida, çivi, dübel çeşitleri', NOW()),
(10, 'Outdoor Ekipman', 'Kamp malzemeleri, çadır, uyku tulumu', NOW())
ON CONFLICT (id) DO NOTHING;

-- Sequence'i güncelle
SELECT setval('categories_id_seq', (SELECT MAX(id) FROM categories));

-- Kullanıcılar (Django auth_user tablosu) - 12 kullanıcı
-- Şifre: pbkdf2_sha256$... formatında hash'lenmiş "password123"
INSERT INTO auth_user (id, username, email, password, first_name, last_name, is_staff, is_active, is_superuser, date_joined) VALUES
(1, 'admin', 'admin@toolshare.com', '123456', 'Admin', 'User', true, true, true, NOW()),
(2, 'ahmet', 'ahmet@email.com', '123456', 'Ahmet', 'Yılmaz', false, true, false, NOW()),
(3, 'mehmet', 'mehmet@email.com', '123456', 'Mehmet', 'Demir', false, true, false, NOW()),
(4, 'ayse', 'ayse@email.com', '123456', 'Ayşe', 'Kaya', false, true, false, NOW()),
(5, 'fatma', 'fatma@email.com', '123456', 'Fatma', 'Çelik', false, true, false, NOW()),
(6, 'ali', 'ali@email.com', '123456', 'Ali', 'Şahin', false, true, false, NOW()),
(7, 'zeynep', 'zeynep@email.com', '123456', 'Zeynep', 'Yıldız', false, true, false, NOW()),
(8, 'mustafa', 'mustafa@email.com', '123456', 'Mustafa', 'Özkan', false, true, false, NOW()),
(9, 'elif', 'elif@email.com', '123456', 'Elif', 'Arslan', false, true, false, NOW()),
(10, 'hasan', 'hasan@email.com', '123456', 'Hasan', 'Koç', false, true, false, NOW()),
(11, 'esra', 'esra@email.com', '123456', 'Esra', 'Aydın', false, true, false, NOW()),
(12, 'burak', 'burak@email.com', '123456', 'Burak', 'Polat', false, true, false, NOW()),
(13, 'bburak', 'burak4@email.com', '123456', 'Burak', 'Polat', false, true, false, NOW())
ON CONFLICT (id) DO NOTHING;

SELECT setval('auth_user_id_seq', (SELECT MAX(id) FROM auth_user));

-- Kullanıcı Profilleri (12 kayıt)
INSERT INTO user_profiles (id, user_id, role, phone, address, trust_score, created_at, updated_at) VALUES
(1, 1, 'admin', '0532 111 1111', 'Kadıköy, İstanbul', 5.00, NOW(), NOW()),
(2, 2, 'user', '0533 222 2222', 'Beşiktaş, İstanbul', 4.50, NOW(), NOW()),
(3, 3, 'user', '0534 333 3333', 'Üsküdar, İstanbul', 4.20, NOW(), NOW()),
(4, 4, 'user', '0535 444 4444', 'Bakırköy, İstanbul', 4.80, NOW(), NOW()),
(5, 5, 'user', '0536 555 5555', 'Şişli, İstanbul', 3.90, NOW(), NOW()),
(6, 6, 'user', '0537 666 6666', 'Maltepe, İstanbul', 4.60, NOW(), NOW()),
(7, 7, 'user', '0538 777 7777', 'Ataşehir, İstanbul', 4.10, NOW(), NOW()),
(8, 8, 'user', '0539 888 8888', 'Kartal, İstanbul', 3.70, NOW(), NOW()),
(9, 9, 'user', '0531 999 9999', 'Pendik, İstanbul', 4.40, NOW(), NOW()),
(10, 10, 'user', '0532 000 0000', 'Tuzla, İstanbul', 4.00, NOW(), NOW()),
(11, 11, 'admin', '0533 111 2222', 'Beyoğlu, İstanbul', 5.00, NOW(), NOW()),
(12, 12, 'user', '0534 222 3333', 'Sarıyer, İstanbul', 3.50, NOW(), NOW())
ON CONFLICT (id) DO NOTHING;

SELECT setval('user_profiles_id_seq', (SELECT MAX(id) FROM user_profiles));

-- Aletler (15 kayıt)
INSERT INTO tools (id, name, description, category_id, owner_id, status, daily_price, created_at, updated_at) VALUES
(1, 'Bosch Profesyonel Matkap', '18V akülü darbeli matkap, 2 adet batarya dahil', 1, 2, 'available', 50.00, NOW(), NOW()),
(2, 'Makita Çim Biçme Makinesi', 'Benzinli, 46cm kesim genişliği', 2, 3, 'available', 80.00, NOW(), NOW()),
(3, 'Stanley El Aleti Seti', '150 parça komple set, çantalı', 3, 4, 'reserved', 30.00, NOW(), NOW()),
(4, 'Wagner Boya Tabancası', 'Elektrikli airless boya tabancası', 4, 5, 'available', 45.00, NOW(), NOW()),
(5, 'Karcher Basınçlı Yıkama', 'K5 Premium, 145 bar basınç', 5, 2, 'available', 60.00, NOW(), NOW()),
(6, 'Bosch Lazer Metre', 'GLM 50 C, Bluetooth bağlantılı', 6, 6, 'available', 25.00, NOW(), NOW()),
(7, 'Alüminyum Merdiven 3 Basamak', 'Katlanır, 150kg taşıma kapasitesi', 7, 7, 'maintenance', 20.00, NOW(), NOW()),
(8, 'Inverter Kaynak Makinesi', '200A, MMA/TIG kaynak', 8, 8, 'available', 70.00, NOW(), NOW()),
(9, 'Dewalt Şarjlı Testere', '20V MAX, daire testere', 1, 3, 'available', 55.00, NOW(), NOW()),
(10, 'Gardena Budama Makası', 'Akülü teleskopik budama', 2, 4, 'available', 35.00, NOW(), NOW()),
(11, 'Hilti Kırıcı Delici', 'TE 30-A36, profesyonel', 1, 6, 'reserved', 90.00, NOW(), NOW()),
(12, 'Basınçlı Su Tabancası', '200 bar, endüstriyel tip', 5, 9, 'available', 40.00, NOW(), NOW()),
(13, 'Alüminyum İskele Seti', '4m yükseklik, tekerlekli', 7, 10, 'available', 100.00, NOW(), NOW()),
(14, 'Profesyonel Taşlama', 'Makita 230mm, 2400W', 1, 8, 'available', 45.00, NOW(), NOW()),
(15, 'Kamp Çadırı 4 Kişilik', 'Su geçirmez, çift katmanlı', 10, 11, 'available', 35.00, NOW(), NOW())
ON CONFLICT (id) DO NOTHING;

SELECT setval('tools_id_seq', (SELECT MAX(id) FROM tools));

-- Rezervasyonlar (15 kayıt)
INSERT INTO reservations (id, tool_id, borrower_id, start_date, end_date, status, total_price, notes, created_at, updated_at) VALUES
(1, 1, 4, '2024-01-10', '2024-01-12', 'completed', 150.00, 'Ev tadilatı için', NOW(), NOW()),
(2, 2, 5, '2024-01-15', '2024-01-15', 'completed', 80.00, 'Bahçe düzenleme', NOW(), NOW()),
(3, 3, 6, '2024-01-20', '2024-01-22', 'completed', 90.00, NULL, NOW(), NOW()),
(4, 4, 7, '2024-02-01', '2024-02-03', 'completed', 135.00, 'Dış cephe boyama', NOW(), NOW()),
(5, 5, 8, '2024-02-10', '2024-02-11', 'completed', 120.00, 'Araba yıkama', NOW(), NOW()),
(6, 6, 9, '2024-02-15', '2024-02-15', 'completed', 25.00, 'Ölçüm işi', NOW(), NOW()),
(7, 8, 10, '2024-02-20', '2024-02-22', 'completed', 210.00, 'Demir korkuluk yapımı', NOW(), NOW()),
(8, 9, 2, '2024-03-01', '2024-03-02', 'completed', 110.00, NULL, NOW(), NOW()),
(9, 10, 3, '2024-03-10', '2024-03-12', 'completed', 105.00, 'Ağaç budama', NOW(), NOW()),
(10, 1, 7, '2024-03-15', '2024-03-17', 'completed', 150.00, 'Mutfak dolabı montajı', NOW(), NOW()),
(11, 3, 8, CURRENT_DATE + 1, CURRENT_DATE + 3, 'approved', 90.00, 'Hafta sonu projesi', NOW(), NOW()),
(12, 11, 9, CURRENT_DATE + 2, CURRENT_DATE + 4, 'approved', 270.00, 'Beton delme işi', NOW(), NOW()),
(13, 5, 10, CURRENT_DATE + 5, CURRENT_DATE + 6, 'pending', 120.00, NULL, NOW(), NOW()),
(14, 13, 4, CURRENT_DATE + 7, CURRENT_DATE + 10, 'pending', 400.00, 'Dış cephe işi', NOW(), NOW()),
(15, 15, 6, CURRENT_DATE + 14, CURRENT_DATE + 16, 'pending', 105.00, 'Kamp gezisi', NOW(), NOW())
ON CONFLICT (id) DO NOTHING;

SELECT setval('reservations_id_seq', (SELECT MAX(id) FROM reservations));

-- Değerlendirmeler (10 kayıt - tamamlanmış rezervasyonlar için)
INSERT INTO ratings (id, reservation_id, score, comment, created_at) VALUES
(1, 1, 5, 'Matkap çok iyi çalıştı, teşekkürler!', NOW()),
(2, 2, 4, 'Makine güzel ama biraz ağırdı', NOW()),
(3, 3, 5, 'Komple set, hiçbir parça eksik değildi', NOW()),
(4, 4, 4, 'Boya tabancası temizdi ve sorunsuz çalıştı', NOW()),
(5, 5, 5, 'Karcher harika, arabam pırıl pırıl oldu', NOW()),
(6, 6, 5, 'Lazer metre çok hassas ölçüm yaptı', NOW()),
(7, 7, 4, 'Kaynak makinesi güçlü, sadece kablo biraz kısa', NOW()),
(8, 8, 5, 'Testere keskin ve güvenli', NOW()),
(9, 9, 4, 'Budama makası işimi gördü', NOW()),
(10, 10, 5, 'Her şey mükemmeldi, tekrar kiralarım', NOW())
ON CONFLICT (id) DO NOTHING;

SELECT setval('ratings_id_seq', (SELECT MAX(id) FROM ratings));

-- =====================================================
-- SEQUENCE kontrolü için ekstra SQL
-- =====================================================
-- Rezervasyon tablosu için özel sequence (gereksinim 8)
CREATE SEQUENCE IF NOT EXISTS reservation_number_seq START 1001;

-- View ve fonksiyon testleri için örnek sorgular (yorum olarak)
-- SELECT * FROM v_tool_details;
-- SELECT * FROM v_user_statistics;
-- SELECT * FROM v_active_reservations;
-- SELECT * FROM get_tool_availability(1, '2024-04-01', '2024-04-03');
-- SELECT * FROM calculate_user_trust_score(2);
-- SELECT * FROM get_category_statistics();
-- SELECT * FROM search_tools('matkap');
