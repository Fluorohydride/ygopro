--ソニック・シューター
function c40384720.initial_effect(c)
	--direct attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetCondition(c40384720.dircon)
	c:RegisterEffect(e1)
	--damage reduce
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EVENT_PRE_BATTLE_DAMAGE)
	e2:SetCondition(c40384720.rdcon)
	e2:SetOperation(c40384720.rdop)
	c:RegisterEffect(e2)
end
function c40384720.dfilter(c)
	return c:GetSequence()<5
end
function c40384720.dircon(e)
	return not Duel.IsExistingMatchingCard(c40384720.dfilter,e:GetHandlerPlayer(),0,LOCATION_SZONE,1,nil)
end
function c40384720.rdcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return ep~=tp and c==Duel.GetAttacker() and Duel.GetAttackTarget()==nil
		and c:GetEffectCount(EFFECT_DIRECT_ATTACK)<2 and Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>0
end
function c40384720.rdop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(ep,e:GetHandler():GetBaseAttack())
end
