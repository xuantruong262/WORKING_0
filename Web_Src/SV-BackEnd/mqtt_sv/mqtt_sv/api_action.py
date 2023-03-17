from . import config
from django.http import JsonResponse
from bson import json_util
import json
from django.core.serializers.json import DjangoJSONEncoder

mqtt_collection = config.MQTT_COLLECTION


def delete_all(request):
    if len(list(mqtt_collection.find())) == 0:
        return JsonResponse({"error": "Collection is already empty"})
    elif request.method == 'GET':
        mqtt_collection.delete_many({})
        return JsonResponse({"error": "Delete all successfully"})
    return JsonResponse({"error": "Failed to deleted all"})


def delete_by_id(request, id):
    if len(list(mqtt_collection.find())) == 0:
        return JsonResponse({"error": "Collection is already empty"})
    elif request.method == 'GET':
        mqtt_collection.delete_one({'device_id': id})
        return JsonResponse({"error": "Delete successfully"})
    return JsonResponse({f'"error": "Failed to deleted device with id: {id}"'})

def get_by_date(request, date):
    if len(list(mqtt_collection.find())) == 0:
        return JsonResponse({"error": "Collection is empty"})
    elif request.method == 'GET':
        day, month, year = split_date(date)
        res = list(mqtt_collection.find({'saved_time.day': day, 'saved_time.month': month, 'saved_time.year': year}))
        for i in res:
            i.pop('_id')

        return JsonResponse(json.dumps(res), safe=False)

def get_range_date(request, from_date, to_date):
    if len(list(mqtt_collection.find())) == 0:
        return JsonResponse({"error": "Collection is empty"})
    elif request.method == 'GET':
        from_day, from_month, from_year = split_date(from_date)
        to_day, to_month, to_year = split_date(to_date)

        query = {
            "saved_time": {
                "$gte": {"day": from_day, "month": from_month, "year": from_year},
                "$lte": {"day": to_day, "month": to_month, "year": to_year}
            }
        }

        res = list(mqtt_collection.find(query))

        for i in res:
            i.pop('_id')

        return JsonResponse(json.dumps(res), safe=False)

def split_date(date):
    date_split = date.split('-')
    day = int(date_split[0])
    month = int(date_split[1])
    year = int(date_split[2])
    return day, month, year