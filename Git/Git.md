# Création repo local vide
Pour créer un dépôt (repository) git local vide :

## 1. Ouvrir un terminal (cmd).

## 2. Aller dans le dossier où je veux créer le dépôt :
```bash
cd /chemin/../dossier
```
## 3. Créer le dossier du projet :
```bash
mkdir mon-projet
cd mon-projet
```
## 4. Initialiser un dépôt git vide :
```bash
git init
```

Après cette commande, un nouveau dépôt **vide** est créé dans ton dossier courant. Un dossier caché `.git` contient les méta-informations du dépôt.

**Remarque** :  
À ce stade, il n’y a aucun fichier suivi par git, le dépôt est totalement vide (sauf le dossier `.git`).

___
# Comment relier mon repo local à github ?
## 1. Créer un nouveau repo sur github
- Aller sur [github.com](https://github.com/)
- Cliquer sur **"New"** ou **"Créer un nouveau repository"**
- Entrer le nom du dépôt, configurer si public/privé
- **Ne pas cocher** "Initialize this repository with a README" (Mais ne pas oublier d'en créer un)
## 2. Copie l’URL du dépôt github

Sur la page du dépôt, copie l’URL fournie de type  
https://github.com/ton-nom-utilisateur/nom-du-repo.git 
## 3. Relier le dépôt local au dépôt distant
Ouvrir un terminal dans le dossier de ton dépôt local, puis taper :
```bash
git remote add origin URL_DU_DEPOT
```
Exemple avec HTTPS :
```bash
git remote add origin https://github.com/MonPseudo/MonRepo.git
```
## 4. Vérifie la connexion
```bash
git remote -v
```
On doit voir origin listé avec l’URL qu'on vient d’ajouter.

---
# .gitignore
La syntaxe d’un fichier **.gitignore** permet de spécifier des fichiers ou dossiers que Git doit ignorer (ne pas suivre). Voici les règles principales :
## 1. **Chaque ligne représente un motif à ignorer**
Exemple :
```
*.log          # Ignore tous les fichiers .log
/temp          # Ignore le dossier temp à la racine
debug.txt      # Ignore le fichier debug.txt à la racine
```
## 2. **Motifs génériques**
- `*` : n’importe quelle suite de caractères  
- `?` : un seul caractère  
- `[abc]` : un des caractères entre crochets  
- `**` : traverse tous les sous-dossiers (ex : `**/cache`)
## 3. **Ignorer des répertoires**
Ajoute `/` à la fin pour viser un dossier.
```
build/         # Ignore le dossier build/ à la racine
```
## 4. **Emplacement et portée**
- Sans `/` devant → s’applique partout dans le repo  
   `*.tmp` ignore tous les fichiers .tmp partout.
- Avec `/` devant → ne s’applique qu’à la racine  
   `/config.php` ignore uniquement `config.php` à la racine du repo.
## 5. **Annulation d’un motif (forcer l’inclusion)**
- Utilise `!` devant.
```
*.log      # Ignore tous les fichiers log
!important.log  # MAIS garde important.log
```
## 6. **Commentaires**
Utilise `#` au début d’une ligne.
```
# Ignorer les fichiers temporaires
*.tmp
```
## 7. **Exemple avec mon .gitignore**
```
# Fichiers et répertoires à ignorer en GConf
.gitignore
.obsidian/
Welcome.md
READMEold.md
libraries/
Git/
Software/FSM/
Software/alarm_clock2/
Software/lcd/
Software/special_char
*.oni
```

---
# Récupérer le projet depuis github
Il faut cloner le dépôt github
Ne pas créer de répertoire vide ni faire de git init.
Faire :
```sh
git clone https://github.com/nom_utilisateur/nom_du_repo.git
```

=> Cela crée un dossier avec tous les fichiers du projet github qui sont rapatriés en local

---
# comment initialiser git pour que la branche initiale s'appelle main et pas master ?
Pour que la branche initiale d'un dépôt Git s'appelle **`main`** au lieu de **`master`**, vous pouvez configurer Git une seule fois à l'aide de la commande suivante :

```bash
git config --global init.defaultBranch main
```

Cette commande définit `main` comme nom de branche par défaut pour tous les nouveaux dépôts créés avec `git init`, à condition d'utiliser une version de Git supérieure ou égale à la 2.28 .([git-scm.com](https://git-scm.com/book/ms/v2/Getting-Started-First-Time-Git-Setup?utm_source=chatgpt.com "1.6 Getting Started - First-Time Git Setup"))

### ✅ Exemple complet

Après avoir exécuté la commande ci-dessus, vous pouvez créer un nouveau dépôt avec :

```bash
mkdir mon-projet
cd mon-projet
git init
```

Cela initialisera un dépôt avec une branche nommée `main`.


### ⚠️ Pour les dépôts existants

Si vous avez déjà un dépôt avec une branche `master` et souhaitez la renommer en `main`, voici les étapes à suivre :

```bash
git branch -m master main
git push -u origin main
git push origin --delete master
```

Ensuite, sur la plateforme d'hébergement (comme GitHub ou GitLab), vous devrez peut-être définir `main` comme branche par défaut et supprimer `master` si nécessaire .


### 🛠️ Pour automatiser avec un alias

Si vous créez fréquemment de nouveaux dépôts et souhaitez automatiser le processus, vous pouvez ajouter un alias à votre configuration Git :

```bash
git config --global alias.new '!git init && git symbolic-ref HEAD refs/heads/main'
```

Ainsi, en exécutant `git new mon-projet`, vous initialiserez un dépôt avec une branche `main` dès le départ .
