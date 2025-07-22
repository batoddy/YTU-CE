import pandas as pd
import os


class Person:
    name = ""
    id = 0


class CalorieDataFrame:
    def __init__(
        self,
        date="",
        workouts=None,
        workouts_kcal=0,
        steps=0,
        steps_kcal=0,
        breakfast="",
        breakfast_kcal=0,
        launch="",
        launch_kcal=0,
        dinner="",
        dinner_kcal=0,
        aperatives="",
        aperatives_kcal=0,
    ):
        self.date = date
        self.workouts = workouts or []
        self.workouts_kcal = workouts_kcal
        self.steps = steps
        self.steps_kcal = steps_kcal
        self.breakfast = breakfast
        self.breakfast_kcal = breakfast_kcal
        self.launch = launch
        self.launch_kcal = launch_kcal
        self.dinner = dinner
        self.dinner_kcal = dinner_kcal
        self.aperatives = aperatives
        self.aperatives_kcal = aperatives_kcal

    def to_dict(self):
        return {
            "Date": self.date,
            "Workouts": ", ".join(self.workouts),
            "Workouts_kcal": self.workouts_kcal,
            "Steps": self.steps,
            "Steps_kcal": self.steps_kcal,
            "Breakfast": self.breakfast,
            "Breakfast_kcal": self.breakfast_kcal,
            "Launch": self.launch,
            "Launch_kcal": self.launch_kcal,
            "Dinner": self.dinner,
            "Dinner_kcal": self.dinner_kcal,
            "Aperatives": self.aperatives,
            "Aperatives_kcal": self.aperatives_kcal,
        }


def create_user_files(person):
    file_name = f"User_{person.id}_data"
    os.makedirs(file_name, exist_ok=True)

    # İlk satırı başlık olacak şekilde boş dataframe
    columns = [
        "Date",
        "Workouts",
        "Workouts_kcal",
        "Steps",
        "Steps_kcal",
        "Breakfast",
        "Breakfast_kcal",
        "Launch",
        "Launch_kcal",
        "Dinner",
        "Dinner_kcal",
        "Aperatives",
        "Aperatives_kcal",
    ]
    df = pd.DataFrame(columns=columns)
    df.to_excel(f"{file_name}/calori_list.xlsx", index=False)


def add_calorie_to_DB(calorie_df, person):
    file_name = f"User_{person.id}_data/calori_list.xlsx"

    # Dosya varsa oku, yoksa boş başlıklarla başlat
    if os.path.exists(file_name):
        df = pd.read_excel(file_name)
    else:
        df = pd.DataFrame(columns=calorie_df.to_dict().keys())

    # Yeni veriyi dict olarak al
    new_row_dict = calorie_df.to_dict()
    new_date_str = pd.to_datetime(new_row_dict["Date"]).strftime("%Y-%m-%d")

    # Eğer "Date" sütunu varsa string'e çevir
    if "Date" in df.columns and not df.empty:
        df["Date"] = pd.to_datetime(df["Date"], errors="coerce").dt.strftime("%Y-%m-%d")

    # Aynı tarih varsa sadece boş olmayanları güncelle
    if new_date_str in df["Date"].values:
        idx = df[df["Date"] == new_date_str].index[0]
        for key, value in new_row_dict.items():
            # Sadece boş olmayan değerleri güncelle
            if value not in ["", 0, None]:
                df.at[idx, key] = value
    else:
        df = pd.concat([df, pd.DataFrame([new_row_dict])], ignore_index=True)

    # Tarihe göre sırala
    df["Date"] = pd.to_datetime(df["Date"], errors="coerce")
    df = df.sort_values("Date").reset_index(drop=True)
    df["Date"] = df["Date"].dt.strftime("%Y-%m-%d")

    # Excel'e yaz
    df.to_excel(file_name, index=False)


def delete_calorie_by_date(date_str, person):
    file_name = f"User_{person.id}_data/calori_list.xlsx"

    # Dosya mevcut mu kontrol et
    if not os.path.exists(file_name):
        print("No data file found for this user.")
        return

    # Excel dosyasını oku
    df = pd.read_excel(file_name)

    # Belirtilen tarihteki satırı sil
    initial_len = len(df)
    df = df[df["Date"] != date_str]

    # Eğer hiç satır silinmemişse kullanıcıyı bilgilendir
    if len(df) == initial_len:
        print(f"No entry found for date {date_str}.")
    else:
        # Güncellenmiş veriyi kaydet
        df.to_excel(file_name, index=False)
        print(f"Entry for date {date_str} deleted successfully.")


calorie_df = CalorieDataFrame(
    date="2025-05-01",
    workouts=["Running", "Pushups"],
    workouts_kcal=500,
    steps=8000,
    steps_kcal=200,
    breakfast="Oatmeal",
    breakfast_kcal=250,
    launch="Chicken Salad",
    launch_kcal=400,
    dinner="Grilled Fish",
    dinner_kcal=350,
    aperatives="Nuts",
    aperatives_kcal=150,
)
# p = Person()
# p.name = "Batuhan"
# p.id = 1

# create_user_files(p)


# delete_calorie_by_date("2025-04-01", p)
# add_calorie_to_DB(calorie_df, p)
