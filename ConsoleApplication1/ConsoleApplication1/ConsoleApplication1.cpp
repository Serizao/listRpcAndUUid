#include <iostream>
#include <windows.h>
#include <rpc.h>
#include <rpcdce.h>

int main() {
    // Déclaration des variables
    RPC_STATUS status;
    RPC_BINDING_HANDLE hBinding = NULL;
    const char* pszStringBinding = "ncacn_ip_tcp:192.168.122.100"; // Chaîne de connexion RPC
    UUID ObjectUUID; // UUID de l'objet RPC
    RPC_EP_INQ_HANDLE hEpInquiry = NULL;
    RPC_IF_ID ifId; // ID de l'interface RPC

    // Connexion RPC
    status = RpcBindingFromStringBindingA((RPC_CSTR)pszStringBinding, &hBinding);
    if (status != RPC_S_OK) {
        std::cout << "Erreur lors de la connexion RPC." << std::endl;
        return 1;
    }

    // Récupérer l'UUID de l'objet RPC
    status = RpcBindingInqObject(hBinding, &ObjectUUID);
    if (status != RPC_S_OK) {
        std::cout << "Erreur lors de la récupération de l'UUID de l'objet RPC." << std::endl;
        RpcBindingFree(&hBinding);
        return 1;
    }

    // Récupérer l'ID de l'interface RPC
    status = RpcIfInqId((RPC_IF_HANDLE)&ObjectUUID, &ifId);
    if (status != RPC_S_OK) {
        std::cout << "Erreur lors de la récupération de l'ID de l'interface RPC." << std::endl;
        RpcBindingFree(&hBinding);
        return 1;
    }

    // Récupérer les informations sur les points de terminaison disponibles
    status = RpcMgmtEpEltInqBegin(hBinding, RPC_C_EP_ALL_ELTS, &ifId, RPC_C_VERS_ALL, NULL, &hEpInquiry);
    if (status != RPC_S_OK) {
        std::cout << "Erreur lors de la récupération des informations sur les points de terminaison." << std::endl;
        RpcBindingFree(&hBinding);
        return 1;
    }

    // Parcourir les points de terminaison disponibles et les afficher
    RPC_IF_ID ifIdFound;
    RPC_BINDING_HANDLE hEpBinding;
    while (RpcMgmtEpEltInqNextA(hEpInquiry, &ifIdFound, &hEpBinding, NULL, NULL) == RPC_S_OK) {
        // Convertir l'UUID en chaîne de caractères
        RPC_CSTR pszUuidString;
        status = UuidToStringA(&ifIdFound.Uuid, &pszUuidString);
        if (status == RPC_S_OK) {
            // Afficher l'UUID de l'interface
            std::cout << "Interface trouvée avec l'UUID : " << pszUuidString << std::endl;

            // Libérer la mémoire allouée pour la chaîne de caractères de l'UUID
            RpcStringFreeA(&pszUuidString);
        }

        // Récupérer le nom de l'interface
        RPC_CSTR pszInterfaceName;
        status = RpcEpResolveBinding(hEpBinding, (RPC_IF_HANDLE)&ifIdFound);
        if (status == RPC_S_OK) {
            status = RpcBindingToStringBindingA(hEpBinding, &pszInterfaceName);
            if (status == RPC_S_OK) {
                // Afficher le nom de l'interface
                std::cout << "Nom de l'interface : " << pszInterfaceName << std::endl;

                // Libérer la mémoire allouée pour le nom de l'interface
                RpcStringFreeA(&pszInterfaceName);
            }
        }

        // Fermer la liaison du point de terminaison
        RpcBindingFree(&hEpBinding);
    }

    // Terminer l'inquiry des points de terminaison
    RpcMgmtEpEltInqDone(&hEpInquiry);

    // Libération des ressources
    status = RpcBindingFree(&hBinding);
    if (status != RPC_S_OK) {
        std::cout << "Erreur lors de la libération de la connexion RPC." << std::endl;
        return 1;
    }

    return 0;
}