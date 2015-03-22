--ターボ・ウォリアー
function c46195773.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,c46195773.tfilter,aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--atk down
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(46195773,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetTarget(c46195773.atktg)
	e1:SetOperation(c46195773.atkop)
	c:RegisterEffect(e1)
	--cannot be target
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetValue(c46195773.efilter)
	c:RegisterEffect(e2)
end
function c46195773.tfilter(c)
	return c:IsCode(67270095) or c:IsHasEffect(20932152)
end
function c46195773.efilter(e,re,rp)
	return re:GetHandler():IsLevelBelow(6) and aux.tgval(e,re,rp)
end
function c46195773.atktg(e,tp,eg,ep,ev,re,r,rp,chk)
	local d=Duel.GetAttackTarget()
	if chk==0 then return d and d:IsFaceup() and d:GetLevel()>=6 and d:IsType(TYPE_SYNCHRO) end
	d:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,d,1,0,0)
end
function c46195773.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local d=Duel.GetAttackTarget()
	if d:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK)
		e1:SetValue(d:GetAttack()/2)
		e1:SetReset(RESET_PHASE+PHASE_DAMAGE)
		d:RegisterEffect(e1)
	end
end
