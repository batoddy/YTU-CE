-- =====================================================
-- SQL FONKSİYONLARI
-- Gereksinim 11: Arayüzden girilen değerleri parametre olarak alıp ekrana sonuç döndüren 
-- 3 farklı SQL fonksiyonu tanımlanmış olmalısınız. 
-- Bu fonksiyonların en az birinde "record" ve "cursor" tanımı-kullanımı olmalıdır.
-- =====================================================

-- =====================================================
-- FONKSİYON 1: get_tool_availability (Basit fonksiyon)
-- Bir aletin belirli tarih aralığında müsait olup olmadığını kontrol eder
-- =====================================================
CREATE OR REPLACE FUNCTION get_tool_availability(
    p_tool_id INTEGER,
    p_start_date DATE,
    p_end_date DATE
)
RETURNS TABLE(
    is_available BOOLEAN,
    conflicting_reservations INTEGER,
    message TEXT
) AS $$
DECLARE
    v_conflict_count INTEGER;
    v_tool_status VARCHAR(20);
BEGIN
    -- Aletin mevcut durumunu kontrol et
    SELECT status INTO v_tool_status FROM tools WHERE id = p_tool_id;
    
    IF v_tool_status IS NULL THEN
        RETURN QUERY SELECT FALSE, 0, 'Alet bulunamadı!'::TEXT;
        RETURN;
    END IF;
    
    IF v_tool_status = 'maintenance' THEN
        RETURN QUERY SELECT FALSE, 0, 'Alet şu anda bakımda!'::TEXT;
        RETURN;
    END IF;
    
    -- Çakışan rezervasyonları say
    SELECT COUNT(*) INTO v_conflict_count
    FROM reservations r
    WHERE r.tool_id = p_tool_id
      AND r.status IN ('pending', 'approved')
      AND NOT (r.end_date < p_start_date OR r.start_date > p_end_date);
    
    IF v_conflict_count > 0 THEN
        RETURN QUERY SELECT FALSE, v_conflict_count, 
            FORMAT('Bu tarih aralığında %s adet çakışan rezervasyon var!', v_conflict_count)::TEXT;
    ELSE
        RETURN QUERY SELECT TRUE, 0, 'Alet bu tarih aralığında müsait!'::TEXT;
    END IF;
END;
$$ LANGUAGE plpgsql;


-- =====================================================
-- FONKSİYON 2: calculate_user_trust_score (RECORD kullanan fonksiyon)
-- Kullanıcının güvenilirlik skorunu hesaplar
-- =====================================================
CREATE OR REPLACE FUNCTION calculate_user_trust_score(p_user_id INTEGER)
RETURNS TABLE(
    user_id INTEGER,
    username VARCHAR,
    completed_rentals INTEGER,
    average_rating DECIMAL(3,2),
    on_time_returns INTEGER,
    calculated_trust_score DECIMAL(3,2),
    trust_level TEXT
) AS $$
DECLARE
    v_user_record RECORD;  -- RECORD kullanımı
    v_completed INTEGER;
    v_avg_rating DECIMAL(3,2);
    v_trust DECIMAL(3,2);
BEGIN
    -- Kullanıcı bilgilerini RECORD'a al
    SELECT u.id, u.username, up.trust_score 
    INTO v_user_record
    FROM auth_user u
    LEFT JOIN user_profiles up ON u.id = up.user_id
    WHERE u.id = p_user_id;
    
    IF v_user_record.id IS NULL THEN
        RAISE EXCEPTION 'Kullanıcı bulunamadı: %', p_user_id;
    END IF;
    
    -- Tamamlanan kiralama sayısı
    SELECT COUNT(*) INTO v_completed
    FROM reservations r
    WHERE r.borrower_id = p_user_id AND r.status = 'completed';
    
    -- Ortalama puan
    SELECT COALESCE(AVG(rt.score), 0)::DECIMAL(3,2) INTO v_avg_rating
    FROM ratings rt
    JOIN reservations r ON rt.reservation_id = r.id
    WHERE r.borrower_id = p_user_id;
    
    -- Güvenilirlik skoru hesapla (basit formül)
    v_trust := LEAST(5.00, (COALESCE(v_avg_rating, 3.0) * 0.6 + LEAST(v_completed, 10) * 0.04 + 2.0))::DECIMAL(3,2);
    
    RETURN QUERY SELECT 
        v_user_record.id,
        v_user_record.username::VARCHAR,
        v_completed,
        v_avg_rating,
        v_completed, -- Basitleştirme için aynı değer
        v_trust,
        CASE 
            WHEN v_trust >= 4.5 THEN 'Çok Güvenilir'
            WHEN v_trust >= 3.5 THEN 'Güvenilir'
            WHEN v_trust >= 2.5 THEN 'Orta'
            ELSE 'Düşük Güvenilirlik'
        END::TEXT;
END;
$$ LANGUAGE plpgsql;


-- =====================================================
-- FONKSİYON 3: get_category_statistics (CURSOR kullanan fonksiyon)
-- Kategorilere göre alet istatistiklerini döndürür - CURSOR kullanımı
-- =====================================================
CREATE OR REPLACE FUNCTION get_category_statistics()
RETURNS TABLE(
    category_id INTEGER,
    category_name VARCHAR,
    tool_count INTEGER,
    available_count INTEGER,
    reserved_count INTEGER,
    total_reservations BIGINT,
    avg_daily_price DECIMAL(10,2)
) AS $$
DECLARE
    category_cursor CURSOR FOR 
        SELECT c.id, c.name FROM categories c ORDER BY c.name;  -- CURSOR tanımı
    v_cat_record RECORD;
    v_tool_count INTEGER;
    v_available INTEGER;
    v_reserved INTEGER;
    v_total_res BIGINT;
    v_avg_price DECIMAL(10,2);
BEGIN
    -- CURSOR'u aç
    OPEN category_cursor;
    
    LOOP
        -- CURSOR'dan veri oku
        FETCH category_cursor INTO v_cat_record;
        EXIT WHEN NOT FOUND;
        
        -- Her kategori için istatistikleri hesapla
        SELECT 
            COUNT(*),
            COUNT(*) FILTER (WHERE status = 'available'),
            COUNT(*) FILTER (WHERE status = 'reserved'),
            COALESCE(AVG(daily_price), 0)
        INTO v_tool_count, v_available, v_reserved, v_avg_price
        FROM tools t
        WHERE t.category_id = v_cat_record.id;
        
        -- Toplam rezervasyon sayısı
        SELECT COUNT(*) INTO v_total_res
        FROM reservations r
        JOIN tools t ON r.tool_id = t.id
        WHERE t.category_id = v_cat_record.id;
        
        -- Sonucu döndür
        category_id := v_cat_record.id;
        category_name := v_cat_record.name;
        tool_count := v_tool_count;
        available_count := v_available;
        reserved_count := v_reserved;
        total_reservations := v_total_res;
        avg_daily_price := v_avg_price;
        
        RETURN NEXT;
    END LOOP;
    
    -- CURSOR'u kapat
    CLOSE category_cursor;
END;
$$ LANGUAGE plpgsql;


-- =====================================================
-- BONUS: Alet arama fonksiyonu (INDEX kullanır)
-- =====================================================
CREATE OR REPLACE FUNCTION search_tools(
    p_search_term VARCHAR DEFAULT NULL,
    p_category_id INTEGER DEFAULT NULL,
    p_status VARCHAR DEFAULT NULL,
    p_max_price DECIMAL DEFAULT NULL
)
RETURNS TABLE(
    tool_id INTEGER,
    tool_name VARCHAR,
    description TEXT,
    category_name VARCHAR,
    owner_name VARCHAR,
    status VARCHAR,
    daily_price DECIMAL,
    average_rating DECIMAL
) AS $$
BEGIN
    RETURN QUERY
    SELECT 
        t.id,
        t.name::VARCHAR,
        t.description,
        c.name::VARCHAR,
        u.username::VARCHAR,
        t.status::VARCHAR,
        t.daily_price,
        COALESCE(
            (SELECT AVG(r.score)::DECIMAL(3,2) 
             FROM ratings r 
             JOIN reservations res ON r.reservation_id = res.id 
             WHERE res.tool_id = t.id), 
            0
        )
    FROM tools t
    JOIN categories c ON t.category_id = c.id
    JOIN auth_user u ON t.owner_id = u.id
    WHERE 
        (p_search_term IS NULL OR t.name ILIKE '%' || p_search_term || '%')
        AND (p_category_id IS NULL OR t.category_id = p_category_id)
        AND (p_status IS NULL OR t.status = p_status)
        AND (p_max_price IS NULL OR t.daily_price <= p_max_price)
    ORDER BY t.name;
END;
$$ LANGUAGE plpgsql;
