--トリッキーズ・マジック4
function c75622824.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c75622824.cost)
	e1:SetTarget(c75622824.target)
	e1:SetOperation(c75622824.activate)
	c:RegisterEffect(e1)
end
function c75622824.cfilter(c)
	return c:IsFaceup() and c:IsCode(14778250) and c:IsAbleToGraveAsCost()
end
function c75622824.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c75622824.cfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c75622824.cfilter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c75622824.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.GetFieldGroupCount(1-tp,LOCATION_MZONE,0)>0 end
	local ct=Duel.GetFieldGroupCount(1-tp,LOCATION_MZONE,0)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,ct,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,ct,0,0)
end
function c75622824.activate(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetFieldGroupCount(1-tp,LOCATION_MZONE,0)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<ct then return end
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,75622825,0,0x4011,2000,1200,5,RACE_SPELLCASTER,ATTRIBUTE_WIND) then return end
	for i=1,ct do
		local token=Duel.CreateToken(tp,75622825)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		token:RegisterEffect(e1,true)
	end
	Duel.SpecialSummonComplete()
end
