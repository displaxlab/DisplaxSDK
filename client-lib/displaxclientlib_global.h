#ifndef DISPLAXCLIENTLIB_GLOBAL_H
#define DISPLAXCLIENTLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DPX_CLIENT_LIB)
#define DPX_EXPORT Q_DECL_EXPORT
#define DPX_HIDDEN Q_DECL_HIDDEN
#else
#define DPX_EXPORT
#define DPX_HIDDEN
#endif

namespace Displax {

class Action;
class Device;
class Entity;
class EntityContext;
class Session;
class SessionManager;

} // namespace Displax

#endif // DISPLAXCLIENTLIB_GLOBAL_H
