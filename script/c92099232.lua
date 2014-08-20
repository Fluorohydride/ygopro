--シェイプシスター
function c92099232.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,92099232+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c92099232.target)
	e1:SetOperation(c92099232.activate)
	c:RegisterEffect(e1)
end
function c92099232.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and 
		Duel.IsPlayerCanSpecialSummonMonster(tp,92099232,0,0x11,0,0,2,RACE_FIEND,ATTRIBUTE_EARTH) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c92099232.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,92099232,0,0x11,0,0,2,RACE_FIEND,ATTRIBUTE_EARTH) then return end
	c:AddTrapMonsterAttribute(TYPE_NORMAL+TYPE_TUNER,ATTRIBUTE_EARTH,RACE_FIEND,2,0,0)
	Duel.SpecialSummon(c,0,tp,tp,true,false,POS_FACEUP)
	c:TrapMonsterBlock()
end
