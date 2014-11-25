--宝玉獣 トパーズ·タイガー
function c95600067.initial_effect(c)
	--send replace
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SEND_REPLACE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c95600067.reptg)
	e1:SetOperation(c95600067.repop)
	c:RegisterEffect(e1)
	--atkup
	local e2=Effect.CreateEffect(c)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c95600067.condition)
	e2:SetValue(400)
	c:RegisterEffect(e2)
end
function c95600067.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetDestination()==LOCATION_GRAVE and e:GetHandler():IsReason(REASON_DESTROY) and Duel.GetLocationCount(tp,LOCATION_SZONE)>0 end
	return Duel.SelectYesNo(tp,aux.Stringid(95600067,0))
end
function c95600067.repop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	Duel.MoveToField(c,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
	local e1=Effect.CreateEffect(c)
	e1:SetCode(EFFECT_CHANGE_TYPE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetReset(RESET_EVENT+0x17c0000)
	e1:SetValue(TYPE_SPELL+TYPE_CONTINUOUS)
	c:RegisterEffect(e1)
	Duel.RaiseEvent(c,47408488,e,0,tp,0,0)
end
function c95600067.condition(e)
	local phase=Duel.GetCurrentPhase()
	return (phase==PHASE_DAMAGE or phase==PHASE_DAMAGE_CAL)
		and Duel.GetAttacker()==e:GetHandler() and Duel.GetAttackTarget()~=nil
end
