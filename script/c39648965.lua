--機皇兵ワイゼル・アイン
function c39648965.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c39648965.val)
	c:RegisterEffect(e1)
	--pierce
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(39648965,0))
	e2:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c39648965.piercecon)
	e2:SetTarget(c39648965.piercetg)
	e2:SetOperation(c39648965.pierceop)
	c:RegisterEffect(e2)
end
function c39648965.atkfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x13)
end
function c39648965.val(e,c)
	return Duel.GetMatchingGroupCount(c39648965.atkfilter,0,LOCATION_MZONE,LOCATION_MZONE,c)*100
end
function c39648965.piercecon(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return d and a:IsControler(tp) and a~=e:GetHandler() and d:IsDefencePos() and a:IsSetCard(0x13)
end
function c39648965.piercetg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.GetAttacker():CreateEffectRelation(e)
end
function c39648965.pierceop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	if a:IsRelateToEffect(e) and a:IsFaceup() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_PIERCE)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		a:RegisterEffect(e1)
	end
end