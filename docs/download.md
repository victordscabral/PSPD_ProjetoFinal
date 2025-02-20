# Guia de Configuração do trabalho Kubernetes
Este guia oferece instruções resumidas e diretas para instalar e configurar o ambiente de trabalho.

---

## 1. Pré-requisitos

- **Docker**
- **kubectl**: v1.32.2
- **kubeadm**: v1.32.2
- **kubelet**: v1.32.2
- **containerd.io**: 1.7.25-1

Certifique-se de atualizar os pacotes antes de iniciar:
```bash
sudo apt update && sudo apt upgrade -y
```

---

## 2. Criação do Usuário Kubernetes

Crie um novo usuário para executar os serviços, vamos padronizar **(no nosso trabalho)** para facilitar documentação e compreendimento:

- **Usuário:** kube
- **Senha:**  kube

Pode deixar em branco (quando for solicitado) informaçoes como:
- Full Name, Room Number, Work Phone, Home Phone, Other.

Atualizar de acordo:
```bash
sudo adduser kube
```
Troque para o novo usuário:
```bash
su - kube
```

### Adicionar o usuário `kube` ao grupo sudo para executar comandos administrativos. Em um usuário com permissões `sudo` como o `root`, utilize:
```bash
sudo usermod -aG sudo kube
```

Verifique se o usuário foi adicionado corretamente ao grupo `sudo`:
```bash
groups kube
```
deve retornar algo como: `kube : kube sudo`


---

</br>
</br>
</br>
</br>

---

## 3. Configuração da Identidade das Máquinas

Após configurada a rede com o switch.

Em cada máquina defina o hostname **(master. worker1, worker2, worker3, ...)**:

```bash
sudo hostnamectl set-hostname master
```

No /etc/hosts também deve-se atualizar:
```bash
sudo nano /etc/hosts
```
Adicione no final do arquivo e salve:
```bash
192.168.1.10 master
192.168.1.11 worker1
192.168.1.12 worker2
192.168.1.13 worker3
```

Agora teremos os ambientes em cada máquina `kube@master`, `kube@worker1` ...

---

## 4. Configuração dos Nós (Master e Workers)

Este passo configura os pré-requisitos para o Kubernetes, incluindo módulos do kernel, pacotes necessários e o container runtime (`containerd`). Essas configurações devem ser feitas **em todas as máquinas** (master e workers).

### **Atualizar pacotes**
```bash
sudo apt-get update && sudo apt-get upgrade -y
```

### **Configurar módulos do kernel**
```bash
cat <<EOF | sudo tee /etc/modules-load.d/containerd.conf
> overlay
> br_netfilter
> EOF
```

```bash
sudo modprobe overlay
```

```bash
sudo modprobe br_netfilter
```

```bash
cat <<EOF | sudo tee /etc/sysctl.d/99-kubernetes-cri.conf
> net.bridge.bridge-nf-call-iptables=1
> net.ipv4.ip_forward=1
> net.bridge.bridge-nf-call-ip6tables=1
> EOF
```

### **Instalar dependências básicas**
```bash
sudo apt-get update
```

```bash
sudo apt-get install curl
```

```bash
sudo apt-get install ca-certificates
```

```bash
sudo apt-get install gnupg
```

```bash
sudo install -m 0755 -d /etc/apt/keyrings
```

### **Adicionar repositório do Docker**
```bash
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
```

```bash
sudo chmod a+r /etc/apt/keyrings/docker.gpg
```

```bash
echo \
> "deb [arch="$(dpkg --print-architecture)" signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
> "$(. /etc/os-release && echo "$VERSION_CODENAME")" stable" | \
> sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

### **Configurar `containerd`**
```bash
sudo apt-get update && sudo apt-get install -y containerd.io
```

```bash
sudo mkdir -p /etc/containerd
```

```bash
sudo containerd config default | sudo tee /etc/containerd/config.toml 
```

```bash
sudo swapoff -a
```

```bash
sudo sed -i '/ swap / s/^\(.*\)$/#\1/' /etc/fstab
```

```bash
sudo systemctl restart containerd
```

```bash
sudo systemctl status containerd
```

### **Adicionar repositório do Kubernetes**
```bash
sudo rm -f /etc/apt/keyrings/kubernetes-apt-keyring.gpg
```

```bash
sudo mkdir -p /etc/apt/keyrings
```

```bash
curl -fsSL https://pkgs.k8s.io/core:/stable:/v1.32/deb/Release.key | \
sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg
```

```bash
echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] \
https://pkgs.k8s.io/core:/stable:/v1.32/deb/ /" | \
sudo tee /etc/apt/sources.list.d/kubernetes.list
```

```bash
sudo apt-get update
```

```bash
sudo apt-get install -y kubelet=1.32.2-1.1 kubeadm=1.32.2-1.1 kubectl=1.32.2-1.1
```

```bash
sudo apt-mark hold kubelet kubeadm kubectl
```


### **Agora apenas na master:**
```bash
sudo sysctl -w net.ipv4.ip_forward=1
```

```bash
sysctl net.ipv4.ip_forward
```

```bash
sudo sysctl --system
```

```bash
sudo kubeadm init --apiserver-advertise-address=192.168.1.10 --pod-network-cidr=192.168.0.0/16 --kubernetes-version=1.32.2
```
esperar terminar de baixar

```bash
mkdir -p $HOME/.kube
```

```bash
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
```

```bash
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

```bash
kubectl get nodes
```

```bash
sudo kubeadm reset -f
sudo rm -rf /etc/cni/net.d
sudo iptables -F && sudo iptables -X
sudo iptables -t nat -F && sudo iptables -t nat -X
sudo iptables -t mangle -F && sudo iptables -t mangle -X
sudo iptables -P FORWARD ACCEPT
sudo systemctl restart kubelet
```

```bash
sudo kubeadm init --apiserver-advertise-address=192.168.1.10 --pod-network-cidr=192.168.0.0/16 --kubernetes-version=1.32.2
```

```bash
rm -rf $HOME/.kube
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

```bash
kubectl apply -f https://raw.githubusercontent.com/projectcalico/calico/v3.25.1/manifests/calico.yaml
```

### **Nas workers para se conectar a master:**

Com o link gerado na master, os workers podem se conectar:

Link parecido com:

```bash
kubeadm join 192.168.1.10:6443 --token lhig5m.wqsmhisg7yyc7ffb \
 --discovery-token-ca-cert-hash sha256:ffbc95629e305b4de054a5de5edf9aa253a93f87bcb2f79a7d84363f539355b4
```