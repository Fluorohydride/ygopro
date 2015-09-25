--エレメント・マジシャン
function c65260293.initial_effect(c)
	--control
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_CHANGE_CONTROL)
	e1:SetCondition(c65260293.ctlcon)
	c:RegisterEffect(e1)
	--chain attack
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(65260293,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_BATTLED)
	e2:SetCondition(c65260293.atcon)
	e2:SetOperation(c65260293.atop)
	c:RegisterEffect(e2)
end
function c65260293.filter(c,att)
	return c:IsFaceup() and c:IsAttribute(att)
end
function c65260293.ctlcon(e)
	return Duel.IsExistingMatchingCard(c65260293.filter,0,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_WATER)
end
function c65260293.atcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return bc and bc:IsStatus(STATUS_BATTLE_DESTROYED) and c:IsChainAttackable() and c:IsStatus(STATUS_OPPO_BATTLE)
		and not c:IsDisabled() and Duel.IsExistingMatchingCard(c65260293.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,ATTRIBUTE_WIND)
end
function c65260293.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack()
end
