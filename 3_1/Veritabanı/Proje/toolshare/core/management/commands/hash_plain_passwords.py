from django.core.management.base import BaseCommand
from django.contrib.auth import get_user_model
from django.contrib.auth.hashers import identify_hasher
import csv
##bu fonksiyon data seed de hash fonksiyonu sorununa takılmamak icin yazılmıstır 

class Command(BaseCommand):
    help = 'Detect plaintext passwords stored in User.password and replace them with Django-hashed passwords.'

    def add_arguments(self, parser):
        parser.add_argument('--default', help='Set this default password for users that have a plaintext password (overwrites the stored value).')
        parser.add_argument('--csv', help='CSV file with username,password rows to apply exact passwords from file.')
        parser.add_argument('--dry-run', action='store_true', help='Show what would be changed without saving.')

    def handle(self, *args, **options):
        User = get_user_model()
        default = options.get('default')
        csv_path = options.get('csv')
        dry = options.get('dry_run')

        csv_map = {}
        if csv_path:
            try:
                with open(csv_path, newline='', encoding='utf-8') as f:
                    reader = csv.reader(f)
                    for row in reader:
                        if len(row) >= 2:
                            csv_map[row[0]] = row[1]
            except Exception as e:
                self.stderr.write(f'Failed to read CSV: {e}')
                return

        updated = 0
        skipped = 0

        for user in User.objects.all():
            pw = (user.password or '')
            # Try to detect hashed password using identify_hasher
            is_hashed = True
            try:
                identify_hasher(pw)
            except Exception:
                is_hashed = False

            if is_hashed:
                skipped += 1
                continue

            # At this point treat stored value as plaintext (or empty)
            new_plain = None
            if csv_map and user.username in csv_map:
                new_plain = csv_map[user.username]
            elif default is not None:
                new_plain = default
            else:
                # If no default or csv provided, assume existing value is plaintext to be re-hashed
                new_plain = pw

            if not new_plain:
                self.stdout.write(f"Skipping {user.username} (no password to set)")
                skipped += 1
                continue

            if dry:
                self.stdout.write(f"Would hash password for {user.username}")
            else:
                user.set_password(new_plain)
                user.save(update_fields=['password'])
                self.stdout.write(self.style.SUCCESS(f"Hashed password for {user.username}"))
                updated += 1

        self.stdout.write(f"Done. Updated: {updated}. Skipped (already hashed/empty): {skipped}.")
