--暴走する魔力
function c50427388.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c50427388.cost)
	e1:SetTarget(c50427388.target)
	e1:SetOperation(c50427388.activate)
	c:RegisterEffect(e1)
end
function c50427388.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c50427388.cfilter(c)
	return c:IsType(TYPE_SPELL) and c:IsAbleToRemove()
end
function c50427388.filter(c,def)
	return c:IsFaceup() and c:IsDefenceBelow(def) and c:IsDestructable()
end
function c50427388.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		local ct=Duel.GetMatchingGroupCount(c50427388.cfilter,tp,LOCATION_GRAVE,0,nil)
		return Duel.IsExistingMatchingCard(c50427388.filter,tp,0,LOCATION_MZONE,1,nil,ct*300)
	end
	local g=Duel.GetMatchingGroup(c50427388.cfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
	e:SetLabel(g:GetCount()*300)
	local sg=Duel.GetMatchingGroup(c50427388.filter,tp,0,LOCATION_MZONE,nil,g:GetCount()*300)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,sg,sg:GetCount(),0,0)
end
function c50427388.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(c50427388.filter,tp,0,LOCATION_MZONE,nil,e:GetLabel())
	Duel.Destroy(sg,REASON_EFFECT)
end
