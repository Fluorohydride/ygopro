--幻魔の殉教者
function c93224848.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c93224848.condition)
	e1:SetCost(c93224848.cost)
	e1:SetTarget(c93224848.target)
	e1:SetOperation(c93224848.activate)
	c:RegisterEffect(e1)
end
function c93224848.filter(c)
	local code=c:GetCode()
	return c:IsFaceup() and (code==6007213 or code==32491822)
end
function c93224848.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>=2
		and Duel.IsExistingMatchingCard(c93224848.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c93224848.cfilter(c)
	return not c:IsAbleToGraveAsCost()
end
function c93224848.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return not Duel.IsExistingMatchingCard(c93224848.cfilter,tp,LOCATION_HAND,0,1,nil) end
	local g=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	Duel.SendtoGrave(g,REASON_COST)
end
function c93224848.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>2
		and Duel.IsPlayerCanSpecialSummonMonster(tp,93224849,0,0x4011,0,0,1,RACE_FIEND,ATTRIBUTE_DARK) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,3,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,3,0,0)
end
function c93224848.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>2
		and Duel.IsPlayerCanSpecialSummonMonster(tp,93224849,0,0x4011,0,0,1,RACE_FIEND,ATTRIBUTE_DARK) then
		for i=1,3 do
			local token=Duel.CreateToken(tp,93224849)
			Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP_ATTACK)
		end
		Duel.SpecialSummonComplete()
	end
end
