//
// Created by ns on 11/16/24.
//

#ifndef ENVELOPEQTPTR_H
#define ENVELOPEQTPTR_H

namespace Gj {

struct EnvelopeQtPtr { long ptr; };

template <class Inspector>
bool inspect(Inspector& f, EnvelopeQtPtr& x) {
    return f.object(x).fields(f.field("ptr", x.ptr));
}

} // Gj

#endif //ENVELOPEQTPTR_H
