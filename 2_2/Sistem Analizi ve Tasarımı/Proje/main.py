import pandas as pd
import os

# Breakfast 12345 + kalori, Launch12345 + kalori,Dinner 12345 + kalori, Aperatives 12345 + kalori, Workout 12345678910 + kalori, Adım sayısı + kalori,


class Person:
    name = ""
    id = 0


class CalorieDataFrame:
    Date = ""
    Workouts = []
    Workouts_kcal = []
    Steps = []
    Steps_kcal = []
    Breakfast = []
    Breakfast_kcal = []
    Launch = []
    Launch_kcal = []
    Dinner = []
    Dinner_kcal = []
    Aperatives = []
    Aperatives_kcal = []


def create_user_files(person):
    file_name = f"User_{person.id}_data"
    os.makedirs(file_name, exist_ok=True)

    calorie_data = {
        "Date": "",
        "Workouts_kcal": 0,
        "Steps": 0,
        "Steps_kcal": 0,
        "Breakfast": "",
        "Breakfast_kcal": 0,
        "Launch": "",
        "Launch_kcal": 0,
        "Dinner": "",
        "Dinner_kcal": 0,
        "Aperatives": "",
        "Aperatives_kcal": 0,
    }

    workout_data = {"Date": "" ""}

    df = pd.DataFrame(calorie_data)

    df.to_excel(f"{file_name}/calori_list.xlsx", index=False)


def add_calorie_to_DB(calorie_df, person):
    file_name = f"User_{person.id}_data"
    


# def add_data():

# def delete_:
calorie_df = CalorieDataFrame()
p = Person()
p.name = "Batuhan"
p.id = 1

create_user_files(p)
