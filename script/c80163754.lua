--バーストブレス
function c80163754.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE+0x1c0)
	e1:SetLabel(1)
	e1:SetCost(c80163754.cost)
	e1:SetTarget(c80163754.target)
	e1:SetOperation(c80163754.activate)
	c:RegisterEffect(e1)
end
function c80163754.cfilter(c,def)
	return c:IsRace(RACE_DRAGON) and c:IsAttackAbove(def)
end
function c80163754.filter(c,atk)
	return c:IsFaceup() and c:IsDestructable() and (not atk or c:IsDefenceBelow(atk))
end
function c80163754.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c80163754.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
		if g:GetCount()==0 then return false end
		local mg,mdef=g:GetMinGroup(Card.GetDefence)
		e:SetLabel(0)
		return Duel.CheckReleaseGroup(tp,c80163754.cfilter,1,nil,mdef)
	end
	local g=Duel.GetMatchingGroup(c80163754.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local mg,mdef=g:GetMinGroup(Card.GetDefence)
	local rg=Duel.SelectReleaseGroup(tp,c80163754.cfilter,1,1,nil,mdef)
	e:SetLabel(rg:GetFirst():GetAttack())
	Duel.Release(rg,REASON_COST)
end
function c80163754.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetLabel()==0 end
	local dg=Duel.GetMatchingGroup(c80163754.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,dg:GetCount(),0,0)
end
function c80163754.activate(e,tp,eg,ep,ev,re,r,rp)
	local dg=Duel.GetMatchingGroup(c80163754.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,e:GetLabel())
	Duel.Destroy(dg,REASON_EFFECT)
end
