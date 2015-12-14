#include "psop_state.h"
#include <map>
using namespace std;
#include "thread/critical_section.h"

static CriticalSection state_lock;
static map<u16, PSOPDATA> state;

void psop_add(const PSOPDATA *opdata)
{
	psop_update(opdata->seqno, opdata);
}

void psop_update(u16 old_seqno, const PSOPDATA *opdata)
{
	CriticalSection::Lock l(state_lock);
	state.erase(old_seqno);
	if(opdata->state != NONE) state.insert(make_pair(opdata->seqno, *opdata));
}

void psop_get(u16 seqno, PSOPDATA *opdata)
{
	CriticalSection::Lock l(state_lock);
	map<u16, PSOPDATA>::iterator it = state.find(seqno);
	if(it != state.end())
	{
		*opdata = it->second;
	}
	else
	{
		opdata->state = NONE;
		opdata->seqno = seqno;
	}
}

void psop_clear_all()
{
	CriticalSection::Lock l(state_lock);
	state.clear();
}

