--ダーク・シムルグ
function c11366199.initial_effect(c)
	--Attribute Dark
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_ADD_ATTRIBUTE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(ATTRIBUTE_WIND)
	c:RegisterEffect(e1)
	--special summon(hand)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(11366199,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_HAND)
	e2:SetCost(c11366199.spcost(LOCATION_GRAVE))
	e2:SetTarget(c11366199.sptg)
	e2:SetOperation(c11366199.spop)
	c:RegisterEffect(e2)
	--special summon(grave)
	local e3=e2:Clone()
	e3:SetRange(LOCATION_GRAVE)
	e3:SetCost(c11366199.spcost(LOCATION_HAND))
	c:RegisterEffect(e3)
	--cannot set
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_CANNOT_MSET)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetRange(LOCATION_MZONE)
	e4:SetTargetRange(0,1)
	e4:SetTarget(aux.TRUE)
	c:RegisterEffect(e4)
	local e5=e4:Clone()
	e5:SetCode(EFFECT_CANNOT_SSET)
	c:RegisterEffect(e5)
	local e6=e4:Clone()
	e6:SetCode(EFFECT_CANNOT_TURN_SET)
	c:RegisterEffect(e6)
	local e7=e4:Clone()
	e7:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e7:SetTarget(c11366199.sumlimit)
	c:RegisterEffect(e7)
end
function c11366199.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	return bit.band(sumpos,POS_FACEDOWN)>0
end
function c11366199.spfilter1(c,tp,loc)
	return c:IsAttribute(ATTRIBUTE_WIND) and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingMatchingCard(c11366199.spfilter2,tp,loc,0,1,c)
end
function c11366199.spfilter2(c)
	return c:IsAttribute(ATTRIBUTE_DARK) and c:IsAbleToRemoveAsCost()
end
function c11366199.spcost(loc)
	return	function(e,tp,eg,ep,ev,re,r,rp,chk)
			if chk==0 then return Duel.IsExistingMatchingCard(c11366199.spfilter1,tp,loc,0,1,nil,tp,loc) end
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
			local g1=Duel.SelectMatchingCard(tp,c11366199.spfilter1,tp,loc,0,1,1,nil,tp,loc)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
			local g2=Duel.SelectMatchingCard(tp,c11366199.spfilter2,tp,loc,0,1,1,g1:GetFirst())
			g1:Merge(g2)
			Duel.Remove(g1,POS_FACEUP,REASON_COST)
		end
end
function c11366199.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c11366199.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 or not c:IsRelateToEffect(e) then return end
	Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
end
