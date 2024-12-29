from locust import HttpUser, TaskSet, task, between

class SearchTasks(TaskSet):
    @task(1)
    def search_query_1(self):
        self.client.get("/search", params={"query": "fine"})

    @task(1)
    def search_query_2(self):
        self.client.get("/search", params={"query": "character"})

    @task(2)
    def search_query_3(self):
        self.client.get("/search", params={"query": "cinema"})

    @task(1)
    def search_query_4(self):
        self.client.get("/search", params={"query": "fiction"})

class WebsiteUser(HttpUser):
    tasks = [SearchTasks]
    wait_time = between(1, 5)