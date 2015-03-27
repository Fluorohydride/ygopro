--光の護封陣
function c69296555.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c69296555.target)
	c:RegisterEffect(e1)
	--race
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c69296555.atktg)
	c:RegisterEffect(e2)
	e1:SetLabelObject(e2)
end
function c69296555.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,563)
	local rc=Duel.AnnounceRace(tp,1,0xffffff)
	e:GetLabelObject():SetLabel(rc)
	e:GetHandler():SetHint(CHINT_RACE,rc)
end
function c69296555.atktg(e,c)
	return c:IsStatus(STATUS_SUMMON_TURN+STATUS_FLIP_SUMMON_TURN) and c:IsRace(e:GetLabel())
end
