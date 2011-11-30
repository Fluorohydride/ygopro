--X－セイバー アクセル
function c53100061.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(53100061,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c53100061.condition)
	e1:SetTarget(c53100061.target)
	e1:SetOperation(c53100061.operation)
	c:RegisterEffect(e1)
end
function c53100061.filter(c)
	return c:IsSetCard(0xd) and c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE)
end
function c53100061.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c53100061.filter,1,nil)
end
function c53100061.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,0,0,tp,1)
end
function c53100061.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
