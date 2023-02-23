from . import config
from django.http import JsonResponse

mqtt_collection = config.MQTT_COLLECTION


def delete_all(request):
    if len(list(mqtt_collection.find())) == 0:
        return JsonResponse({"error": "Collection is already empty"})
    elif request.method == 'DELETE':
        mqtt_collection.delete_many({})
        return JsonResponse({"error": "Delete all successfully"})
    return JsonResponse({"error": "Failed to deleted all"})

def delete_by_id(request, id):
    if len(list(mqtt_collection.find())) == 0:
        return JsonResponse({"error": "Collection is already empty"})
    elif request.method == 'DELETE':
        mqtt_collection.delete_one({"$unset": {"device""temp": 14}})
        return JsonResponse({"error": "Delete successfully"})
    return JsonResponse({"error": "Failed to deleted all"})
