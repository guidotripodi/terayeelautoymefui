#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Agustin Rodriguez y Guido Tripodi");
MODULE_DESCRIPTION("Es primo?");

// Variables globales
int number;

char * es_primo(void) {
    unsigned int i;

    if (number == 0) {
        return "0";
    }

    for (i = 2; i < number; i++) {
        if (number % i == 0) {
            return "0";
        }
    }

    return "1";
}

//Funciones de lectura invocada por /dev fs
static ssize_t esprimo_read(struct file *file, char *buf, size_t count, loff_t *ppos) {
	printk(KERN_INFO "Mi numero %d\n", number);
    printk(KERN_INFO "Es primo %s\n", es_primo());
    copy_to_user(buf, es_primo(), 1);
    return 1;
}

static ssize_t esprimo_write(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    char input[1024];
    copy_from_user(&input, buf, count);
    sscanf(&input, "%d", &number);
    printk(KERN_INFO "esprimo: Se ha cambiado el numero por %d\n", number);
    return count;
}

// Estructuras utililizadas por la funcion de registro de dispositivos
static const struct file_operations esprimo_fops = {
    .owner = THIS_MODULE,
    .read = esprimo_read,
    .write = esprimo_write
};

static struct miscdevice esprimo_dev = {
    MISC_DYNAMIC_MINOR,
    "esprimo",
    &esprimo_fops
};

// Funciones utilizadas por la creacion y destruccion del modulo
static int __init esprimo_init(void) {
    int ret;
    // Registración del device
    ret = misc_register(&esprimo_dev);
    if (ret) {
        printk(KERN_ERR "No se puede registrar el dispositivo ESPRIMO\n");
    } else {
        number = 0;
    }
    return ret;
}

static void __exit esprimo_exit(void) {
    misc_deregister(&esprimo_dev);
}

// Definicion de constructor y destructor del modulo.
module_init(esprimo_init);
module_exit(esprimo_exit);

// Y con "cat /dev/esprimo" para ver el numero almacenado.
