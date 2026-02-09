-- =====================================================
-- VIEW TANIMLARI
-- Gereksinim 6: Arayüzden çağrılan sorgulardan en az biri "view" olarak tanımlanmış olmalıdır
-- Gereksinim 9: UNION, INTERSECT, EXCEPT kullanımı
-- Gereksinim 10: Aggregate fonksiyonlar + HAVING
-- =====================================================

-- =====================================================
-- VIEW 1: Alet detayları
-- =====================================================
CREATE OR REPLACE VIEW v_tool_details AS
SELECT 
    t.id AS tool_id,
    t.name AS tool_name,
    t.description AS tool_description,
    t.status,
    t.daily_price,
    t.created_at,
    c.id AS category_id,
    c.name AS category_name,
    u.id AS owner_id,
    u.username AS owner_username,
    u.email AS owner_email,
    up.phone AS owner_phone,
    up.trust_score AS owner_trust_score,
    COALESCE(
        (SELECT AVG(r.score)::DECIMAL(3,2) 
         FROM ratings r 
         JOIN reservations res ON r.reservation_id = res.id 
         WHERE res.tool_id = t.id), 
        0
    ) AS average_rating,
    (SELECT COUNT(*) FROM reservations WHERE tool_id = t.id AND status = 'completed') AS total_rentals
FROM tools t
JOIN categories c ON t.category_id = c.id
JOIN auth_user u ON t.owner_id = u.id
LEFT JOIN user_profiles up ON u.id = up.user_id;


-- =====================================================
-- VIEW 2: Kullanıcı istatistikleri
-- =====================================================
CREATE OR REPLACE VIEW v_user_statistics AS
SELECT 
    u.id AS user_id,
    u.username,
    u.email,
    up.role,
    up.trust_score,
    (SELECT COUNT(*) FROM tools WHERE owner_id = u.id) AS owned_tools_count,
    (SELECT COUNT(*) FROM reservations WHERE borrower_id = u.id) AS reservations_count,
    (SELECT COUNT(*) FROM reservations WHERE borrower_id = u.id AND status = 'completed') AS completed_rentals,
    (SELECT COALESCE(AVG(r.score), 0) 
     FROM ratings r 
     JOIN reservations res ON r.reservation_id = res.id 
     WHERE res.borrower_id = u.id)::DECIMAL(3,2) AS average_rating_given
FROM auth_user u
LEFT JOIN user_profiles up ON u.id = up.user_id;


-- =====================================================
-- VIEW 3: Aktif rezervasyonlar
-- =====================================================
CREATE OR REPLACE VIEW v_active_reservations AS
SELECT 
    r.id AS reservation_id,
    t.name AS tool_name,
    t.daily_price,
    owner.username AS owner_name,
    borrower.username AS borrower_name,
    r.start_date,
    r.end_date,
    r.status,
    r.total_price,
    (r.end_date - r.start_date + 1) AS rental_days
FROM reservations r
JOIN tools t ON r.tool_id = t.id
JOIN auth_user owner ON t.owner_id = owner.id
JOIN auth_user borrower ON r.borrower_id = borrower.id
WHERE r.status IN ('pending', 'approved');


-- =====================================================
-- VIEW 4: UNION - Müsait ve Rezerve Aletler (Gereksinim 9)
-- =====================================================
CREATE OR REPLACE VIEW v_tools_union AS
SELECT id, name, 'Müsait' AS durum, daily_price 
FROM tools WHERE status = 'available'
UNION
SELECT id, name, 'Rezerve' AS durum, daily_price 
FROM tools WHERE status = 'reserved';


-- =====================================================
-- VIEW 5: INTERSECT - Hem Sahip Hem Kiralayan Kullanıcılar (Gereksinim 9)
-- =====================================================
CREATE OR REPLACE VIEW v_users_intersect AS
SELECT DISTINCT u.id, u.username, u.email 
FROM auth_user u
JOIN tools t ON u.id = t.owner_id
INTERSECT
SELECT DISTINCT u.id, u.username, u.email 
FROM auth_user u
JOIN reservations r ON u.id = r.borrower_id;


-- =====================================================
-- VIEW 6: EXCEPT - Sadece Alet Sahibi Olanlar (Gereksinim 9)
-- =====================================================
CREATE OR REPLACE VIEW v_users_except AS
SELECT DISTINCT u.id, u.username, u.email 
FROM auth_user u
JOIN tools t ON u.id = t.owner_id
EXCEPT
SELECT DISTINCT u.id, u.username, u.email 
FROM auth_user u
JOIN reservations r ON u.id = r.borrower_id;


-- =====================================================
-- VIEW 7: Aggregate + HAVING - Kategori İstatistikleri (Gereksinim 10)
-- =====================================================
CREATE OR REPLACE VIEW v_category_stats AS
SELECT 
    c.id AS category_id,
    c.name AS kategori,
    COUNT(t.id) AS alet_sayisi,
    ROUND(AVG(t.daily_price)::numeric, 2) AS ortalama_fiyat,
    SUM(t.daily_price) AS toplam_fiyat,
    MAX(t.daily_price) AS max_fiyat,
    MIN(t.daily_price) AS min_fiyat
FROM categories c 
JOIN tools t ON c.id = t.category_id
GROUP BY c.id, c.name 
HAVING COUNT(t.id) >= 1;