--炎獣の影霊衣－セフィラエグザ
function c20773176.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--splimit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_PZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetTargetRange(1,0)
	e2:SetTarget(c20773176.splimit)
	e2:SetCondition(c20773176.splimcon)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e3:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e3:SetCode(EVENT_DESTROYED)
	e3:SetCountLimit(1,20773176)
	e3:SetCondition(c20773176.condition)
	e3:SetTarget(c20773176.target)
	e3:SetOperation(c20773176.operation)
	c:RegisterEffect(e3)
end
function c20773176.splimit(e,c,sump,sumtype,sumpos,targetp)
	if c:IsSetCard(0xb4) or c:IsSetCard(0xc4) then return false end
	return bit.band(sumtype,SUMMON_TYPE_PENDULUM)==SUMMON_TYPE_PENDULUM
end
function c20773176.splimcon(e)
	return not e:GetHandler():IsForbidden()
end
function c20773176.filter(c,tp)
	return c:IsReason(REASON_BATTLE+REASON_EFFECT)
		and (c:IsSetCard(0xb4) or c:IsSetCard(0xc4)) and not c:IsCode(20773176)
		and c:GetPreviousControler()==tp
		and ((c:IsPreviousLocation(LOCATION_MZONE) and c:IsPreviousPosition(POS_FACEUP))
		or (c:IsPreviousLocation(LOCATION_SZONE) and (c:GetPreviousSequence()==6 or c:GetPreviousSequence()==7)))
end
function c20773176.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c20773176.filter,1,nil,tp)
end
function c20773176.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c20773176.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,true,false,POS_FACEUP)
	end
end
