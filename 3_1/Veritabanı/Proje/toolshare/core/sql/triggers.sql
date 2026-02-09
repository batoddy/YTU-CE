-- =====================================================
-- TRIGGER TANIMLARI
-- Gereksinim 12: 2 adet trigger tanımlamalı ve arayüzden girilecek değerlerle tetiklemelisiniz.
-- Trigger'ın çalıştığına dair arayüze bilgilendirme mesajı döndürülmelidir.
-- =====================================================

-- =====================================================
-- TRIGGER 1: Rezervasyon oluşturulduğunda tetiklenir
-- Alet durumunu günceller ve audit log'a kayıt ekler
-- =====================================================
CREATE OR REPLACE FUNCTION trg_reservation_insert()
RETURNS TRIGGER AS $$
DECLARE
    v_tool_name VARCHAR;
    v_owner_name VARCHAR;
    v_message TEXT;
BEGIN
    -- Alet bilgilerini al
    SELECT t.name, u.username 
    INTO v_tool_name, v_owner_name
    FROM tools t
    JOIN auth_user u ON t.owner_id = u.id
    WHERE t.id = NEW.tool_id;
    
    -- Bilgilendirme mesajı oluştur
    v_message := FORMAT(
        'TRIGGER BİLGİLENDİRME: Yeni rezervasyon oluşturuldu! Alet: %s, Tarih: %s - %s, Sahip: %s',
        v_tool_name,
        NEW.start_date,
        NEW.end_date,
        v_owner_name
    );
    
    -- Audit log'a kaydet
    INSERT INTO audit_logs (table_name, action, record_id, new_values, message, performed_at)
    VALUES (
        'reservations',
        'INSERT',
        NEW.id,
        jsonb_build_object(
            'tool_id', NEW.tool_id,
            'borrower_id', NEW.borrower_id,
            'start_date', NEW.start_date,
            'end_date', NEW.end_date,
            'status', NEW.status
        ),
        v_message,
        NOW()
    );
    
    -- Bilgilendirme mesajını NOTICE olarak gönder (Django'da yakalanabilir)
    RAISE NOTICE '%', v_message;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Trigger'ı tabloya bağla
DROP TRIGGER IF EXISTS trigger_reservation_insert ON reservations;
CREATE TRIGGER trigger_reservation_insert
    AFTER INSERT ON reservations
    FOR EACH ROW
    EXECUTE FUNCTION trg_reservation_insert();


-- =====================================================
-- TRIGGER 2: Alet durumu güncellendiğinde tetiklenir
-- Durum değişikliğini loglar ve bilgilendirme mesajı üretir
-- =====================================================
CREATE OR REPLACE FUNCTION trg_tool_status_update()
RETURNS TRIGGER AS $$
DECLARE
    v_message TEXT;
    v_owner_email VARCHAR;
BEGIN
    -- Sadece status değiştiğinde çalış
    IF OLD.status IS DISTINCT FROM NEW.status THEN
        -- Sahip email'ini al
        SELECT email INTO v_owner_email
        FROM auth_user
        WHERE id = NEW.owner_id;
        
        -- Bilgilendirme mesajı oluştur
        v_message := FORMAT(
            'TRIGGER BİLGİLENDİRME: Alet durumu güncellendi! Alet: %s (ID: %s), Eski Durum: %s, Yeni Durum: %s, Güncelleme Zamanı: %s',
            NEW.name,
            NEW.id,
            CASE OLD.status
                WHEN 'available' THEN 'Müsait'
                WHEN 'reserved' THEN 'Rezerve'
                WHEN 'maintenance' THEN 'Bakımda'
                ELSE OLD.status
            END,
            CASE NEW.status
                WHEN 'available' THEN 'Müsait'
                WHEN 'reserved' THEN 'Rezerve'
                WHEN 'maintenance' THEN 'Bakımda'
                ELSE NEW.status
            END,
            NOW()
        );
        
        -- Audit log'a kaydet
        INSERT INTO audit_logs (table_name, action, record_id, old_values, new_values, message, performed_at)
        VALUES (
            'tools',
            'UPDATE',
            NEW.id,
            jsonb_build_object('status', OLD.status, 'name', OLD.name),
            jsonb_build_object('status', NEW.status, 'name', NEW.name),
            v_message,
            NOW()
        );
        
        -- Bilgilendirme mesajını NOTICE olarak gönder
        RAISE NOTICE '%', v_message;
    END IF;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Trigger'ı tabloya bağla
DROP TRIGGER IF EXISTS trigger_tool_status_update ON tools;
CREATE TRIGGER trigger_tool_status_update
    AFTER UPDATE ON tools
    FOR EACH ROW
    EXECUTE FUNCTION trg_tool_status_update();


-- =====================================================
-- BONUS TRIGGER: Rating eklendiğinde güvenilirlik skorunu güncelle
-- =====================================================
CREATE OR REPLACE FUNCTION trg_rating_insert()
RETURNS TRIGGER AS $$
DECLARE
    v_borrower_id INTEGER;
    v_new_trust_score DECIMAL(3,2);
    v_avg_rating DECIMAL(3,2);
    v_message TEXT;
BEGIN
    -- Rezervasyondan kiralayan ID'sini al
    SELECT borrower_id INTO v_borrower_id
    FROM reservations
    WHERE id = NEW.reservation_id;
    
    -- Kullanıcının ortalama puanını hesapla
    SELECT COALESCE(AVG(r.score), 3.0)::DECIMAL(3,2) INTO v_avg_rating
    FROM ratings r
    JOIN reservations res ON r.reservation_id = res.id
    WHERE res.borrower_id = v_borrower_id;
    
    -- Yeni güvenilirlik skoru (basit formül)
    v_new_trust_score := LEAST(5.00, (v_avg_rating * 0.8 + 1.0))::DECIMAL(3,2);
    
    -- Kullanıcı profilini güncelle
    UPDATE user_profiles
    SET trust_score = v_new_trust_score,
        updated_at = NOW()
    WHERE user_id = v_borrower_id;
    
    -- Bilgilendirme mesajı
    v_message := FORMAT(
        'TRIGGER BİLGİLENDİRME: Değerlendirme eklendi! Puan: %s/5, Kullanıcı güvenilirlik skoru güncellendi: %s',
        NEW.score,
        v_new_trust_score
    );
    
    RAISE NOTICE '%', v_message;
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Trigger'ı tabloya bağla
DROP TRIGGER IF EXISTS trigger_rating_insert ON ratings;
CREATE TRIGGER trigger_rating_insert
    AFTER INSERT ON ratings
    FOR EACH ROW
    EXECUTE FUNCTION trg_rating_insert();
