--連鎖爆撃
function c91623717.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c91623717.condition)
	e1:SetTarget(c91623717.target)
	e1:SetOperation(c91623717.activate)
	c:RegisterEffect(e1)
end
function c91623717.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()>0 and Duel.CheckChainUniqueness()
end
function c91623717.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	local dam=Duel.GetCurrentChain()*400
	Duel.SetTargetParam(dam)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam)
end
function c91623717.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
