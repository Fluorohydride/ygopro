--魔装機関車 デコイチ
function c87621407.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(87621407,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c87621407.target)
	e1:SetOperation(c87621407.operation)
	c:RegisterEffect(e1)
end
function c87621407.filter(c)
	return c:IsFaceup() and c:IsCode(8715625)
end
function c87621407.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ct=Duel.GetMatchingGroupCount(c87621407.filter,tp,LOCATION_ONFIELD,0,nil)
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(ct+1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,ct+1)
end
function c87621407.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
