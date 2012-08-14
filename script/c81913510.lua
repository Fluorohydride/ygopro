--NEX
function c81913510.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c81913510.cost)
	e1:SetTarget(c81913510.target)
	e1:SetOperation(c81913510.activate)
	c:RegisterEffect(e1)
end
function c81913510.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c81913510.filter1(c,e,tp)
	local code=c:GetCode()
	return c:IsFaceup() and c:IsSetCard(0x1f) and c:IsAbleToGraveAsCost()
		and Duel.IsExistingMatchingCard(c81913510.filter2,tp,LOCATION_EXTRA,0,1,nil,code,e,tp)
end
function c81913510.filter2(c,code,e,tp)
	return c:IsCode(code) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c81913510.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
			and Duel.IsExistingMatchingCard(c81913510.filter1,tp,LOCATION_MZONE,0,1,nil,e,tp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local rg=Duel.SelectMatchingCard(tp,c81913510.filter1,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
	Duel.SendtoGrave(rg,REASON_COST)
	e:SetLabel(rg:GetFirst():GetCode())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c81913510.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local code=e:GetLabel()
	local tc=Duel.GetFirstMatchingCard(c81913510.filter2,tp,LOCATION_EXTRA,0,nil,code,e,tp)
	if tc and Duel.SpecialSummon(tc,0,tp,tp,true,false,POS_FACEUP) then
		tc:CompleteProcedure()
	end
end
