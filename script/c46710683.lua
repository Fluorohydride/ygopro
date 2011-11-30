--BF－銀盾のミストラル
function c46710683.initial_effect(c)
	--change damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(46710683,0))
	e1:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c46710683.condition)
	e1:SetOperation(c46710683.operation)
	c:RegisterEffect(e1)
end
function c46710683.condition(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_DESTROY)>0
		and bit.band(e:GetHandler():GetPreviousLocation(),LOCATION_ONFIELD)>0
end
function c46710683.operation(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,0)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetValue(c46710683.damval)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	c46710683.available=true
end
c46710683.available=false
function c46710683.damval(e,re,val,r,rp,rc)
	if not c46710683.available or bit.band(r,REASON_BATTLE)==0 then return val end
	c46710683.available=false
	return 0
end
