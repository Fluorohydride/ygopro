--ゴーレム
function c17313545.initial_effect(c)
	--disable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(c17313545.distg)
	c:RegisterEffect(e1)
	--chain attack
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(17313545,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_BATTLE_DESTROYING)
	e2:SetCondition(c17313545.atcon)
	e2:SetOperation(c17313545.atop)
	c:RegisterEffect(e2)
end
function c17313545.distg(e,c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsType(TYPE_EFFECT)
end
function c17313545.atcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetBattleTarget():IsAttribute(ATTRIBUTE_LIGHT)
		and e:GetHandler():IsChainAttackable()
end
function c17313545.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChainAttack()
end
