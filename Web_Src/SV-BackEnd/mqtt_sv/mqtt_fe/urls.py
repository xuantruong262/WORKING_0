
from django.urls import path
from . import views

urlpatterns = [
    # path('admin/', admin.site.urls),
    # path('publish', publisher.publish_message, name='publish'),
    # path('mqtt/delete/all', api_action.delete_all, name='delete all'),
    # path('mqtt/delete/<int:id>', api_action.delete_by_id, name='delete all')
    path('', views.home,name="home"),
]
