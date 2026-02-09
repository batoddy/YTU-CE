

DO
$do$
BEGIN
   IF NOT EXISTS (
      SELECT FROM pg_catalog.pg_roles
      WHERE  rolname = 'toolshare_user') THEN
      CREATE ROLE toolshare_user LOGIN PASSWORD '12345';
   ELSE
      ALTER ROLE toolshare_user WITH PASSWORD '12345';
   END IF;
END
$do$;



-- Şema ve Veritabanı Yetkileri (Hatanın Çözümü)
GRANT ALL ON SCHEMA public TO toolshare_user;
GRANT ALL PRIVILEGES ON DATABASE toolshare_db TO toolshare_user;
ALTER DATABASE toolshare_db OWNER TO toolshare_user;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO toolshare_user;
ALTER USER toolshare_user WITH SUPERUSER;