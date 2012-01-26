--闇の閃光
function c29876529.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCost(c29876529.cost)
	e1:SetTarget(c29876529.target)
	e1:SetOperation(c29876529.activate)
	c:RegisterEffect(e1)
end
function c29876529.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c29876529.costfilter(c)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:IsAttackAbove(1500)
		and Duel.IsExistingMatchingCard(c29876529.dfilter,0,LOCATION_MZONE,LOCATION_MZONE,1,c)
end
function c29876529.dfilter(c)
	return c:GetTurnID()==Duel.GetTurnCount() and bit.band(c:GetSummonType(),SUMMON_TYPE_SPECIAL)~=0 and c:IsDestructable()
end
function c29876529.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=0 then
			e:SetLabel(0)
			return Duel.CheckReleaseGroup(tp,c29876529.costfilter,1,nil)
		else
			return Duel.IsExistingMatchingCard(c29876529.dfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil)
		end
	end
	if e:GetLabel()==1 then
		e:SetLabel(0)
		local rg=Duel.SelectReleaseGroup(tp,c29876529.costfilter,1,1,nil)
		Duel.Release(rg,REASON_COST)
	end
	local g=Duel.GetMatchingGroup(c29876529.dfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c29876529.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c29876529.dfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.Destroy(g,REASON_EFFECT)
end
