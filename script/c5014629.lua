--潜航母艦エアロ・シャーク
function c5014629.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,3,2)
	c:EnableReviveLimit()
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(5014629,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c5014629.damcost)
	e1:SetTarget(c5014629.damtg)
	e1:SetOperation(c5014629.damop)
	c:RegisterEffect(e1)
end
function c5014629.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c5014629.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_MONSTER)
end
function c5014629.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c5014629.filter,tp,LOCATION_REMOVED,0,1,nil) end
	local ct=Duel.GetMatchingGroupCount(c5014629.filter,tp,LOCATION_REMOVED,0,nil)
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(ct*100)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,ct*100)
end
function c5014629.damop(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetMatchingGroupCount(c5014629.filter,tp,LOCATION_REMOVED,0,nil)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	Duel.Damage(p,ct*100,REASON_EFFECT)
end
