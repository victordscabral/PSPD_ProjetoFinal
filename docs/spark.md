helm repo add spark-operator https://kubeflow.github.io/spark-operator
helm repo update

helm uninstall spark-operator -n spark-operator

helm install spark-operator spark-operator/spark-operator \
    --namespace spark-operator --create-namespace \
    --set webhook.enable=false



kubectl delete sparkapplication spark-pi -n default

