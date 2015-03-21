--天よりの宝札
function c42664989.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c42664989.cost)
	e1:SetTarget(c42664989.target)
	e1:SetOperation(c42664989.operation)
	c:RegisterEffect(e1)
end
function c42664989.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToRemoveAsCost,tp,LOCATION_HAND+LOCATION_ONFIELD,0,1,e:GetHandler()) end
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemoveAsCost,tp,LOCATION_HAND+LOCATION_ONFIELD,0,e:GetHandler())
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c42664989.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp) end
	local ht=Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(2-ht)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2-ht)
end
function c42664989.operation(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local ht=Duel.GetFieldGroupCount(p,LOCATION_HAND,0)
	if ht<2 then
		Duel.Draw(p,2-ht,REASON_EFFECT)
	end
end
