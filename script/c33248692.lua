--オプションハンター
function c33248692.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_RECOVER)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetRange(LOCATION_SZONE)
	e1:SetCondition(c33248692.condition)
	e1:SetTarget(c33248692.target)
	e1:SetOperation(c33248692.operation)
	c:RegisterEffect(e1)
end
function c33248692.filter(c,tp)
	return c:GetPreviousControler()==tp and c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE)
end
function c33248692.condition(e,tp,eg,ep,ev,re,r,rp,chk)
	return eg:IsExists(c33248692.filter,1,nil,tp)
end
function c33248692.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local rec=eg:Filter(c33248692.filter,nil,tp):GetFirst():GetBaseAttack()
	if rec<0 then rec=0 end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(rec)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,rec)
end
function c33248692.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
