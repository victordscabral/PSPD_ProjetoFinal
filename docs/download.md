# Guia de Configuração do trabalho Kubernetes
Este guia oferece instruções resumidas e diretas para instalar e configurar o ambiente de trabalho.

---

## 1. Pré-requisitos

- **Docker**: 
- **kubectl**: v1.32.2
- **kubeadm**: v1.32.2
- **kubelet**: v1.32.2

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

## 3. Baixando kubectl, kubeadm e kubelet