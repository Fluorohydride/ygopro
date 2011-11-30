--吸収天児
function c49771608.initial_effect(c)
	--recover
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49771608,0))
	e1:SetCategory(CATEGORY_RECOVER)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCondition(c49771608.condition)
	e1:SetTarget(c49771608.target)
	e1:SetOperation(c49771608.operation)
	c:RegisterEffect(e1)
end
function c49771608.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return not c:IsStatus(STATUS_BATTLE_DESTROYED) 
		and ((c==a and d:GetLocation()==LOCATION_GRAVE) or (c==d and a:GetLocation()==LOCATION_GRAVE))
end
function c49771608.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	local m=0
	if a==e:GetHandler() then m=d:GetLevel()*300
	else m=a:GetLevel()*300 end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(m)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,m)
end
function c49771608.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
