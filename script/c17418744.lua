--フォトン·サンクチュアリ
function c17418744.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c17418744.cost)
	e1:SetTarget(c17418744.target)
	e1:SetOperation(c17418744.activate)
	c:RegisterEffect(e1)
end
function c17418744.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return not Duel.SummonedCardsThisTurn(tp):IsExists(Card.IsAttribute,1,nil,0x6f)
		and not Duel.FlipSummonedCardsThisTurn(tp):IsExists(Card.IsAttribute,1,nil,0x6f)
		and not Duel.SpecialSummonedCardsThisTurn(tp):IsExists(Card.IsAttribute,1,nil,0x6f) end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c17418744.sumlimit)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e2:SetCode(EFFECT_CANNOT_SUMMON)
	e2:SetReset(RESET_PHASE+PHASE_END)
	e2:SetTargetRange(1,0)
	e2:SetTarget(c17418744.sumlimit)
	Duel.RegisterEffect(e2,tp)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	Duel.RegisterEffect(e3,tp)
end
function c17418744.sumlimit(e,c,sump,sumtype,sumpos,targetp,se)
	return c:IsAttribute(0x6f)
end
function c17418744.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsPlayerCanSpecialSummonMonster(tp,17418745,0,0x4011,2000,0,1,RACE_THUNDER,ATTRIBUTE_LIGHT)
	end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,0,0)
end
function c17418744.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>1 
		and Duel.IsPlayerCanSpecialSummonMonster(tp,17418745,0,0x4011,2000,0,1,RACE_THUNDER,ATTRIBUTE_LIGHT) then
		for i=1,2 do
			local token=Duel.CreateToken(tp,17418745)
			Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_ATTACK)
			e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			token:RegisterEffect(e1,true)
			local e2=Effect.CreateEffect(e:GetHandler())
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_CANNOT_BE_SYNCHRO_MATERIAL)
			e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
			e2:SetValue(1)
			token:RegisterEffect(e2,true)
		end
		Duel.SpecialSummonComplete()
	end
end
