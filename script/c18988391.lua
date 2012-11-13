--アマリリース
function c18988391.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(18988391,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCost(c18988391.cost)
	e1:SetOperation(c18988391.operation)
	c:RegisterEffect(e1)
end
function c18988391.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,18988391)==0 and e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
	Duel.RegisterFlagEffect(tp,18988391,RESET_PHASE+PHASE_END,0,1)
end
function c18988391.operation(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetDescription(aux.Stringid(18988391,1))
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetTargetRange(LOCATION_HAND,0)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c18988391.sumcon)
	e1:SetTarget(c18988391.efftg)
	e1:SetOperation(c18988391.sumop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c18988391.efftg(e,c)
	local mi,ma=c:GetTributeRequirement()
	return mi>0
end
function c18988391.sumcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local mi,ma=c:GetTributeRequirement()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	return (mi==1 and ft>0 and Duel.IsPlayerCanSummon(tp,SUMMON_TYPE_NORMAL,c))
		or (ma==2 and ft>-1 and Duel.IsPlayerCanSummon(tp,SUMMON_TYPE_ADVANCE,c) and Duel.GetTributeCount(c)>=1)
end
function c18988391.sumop(e,tp,eg,ep,ev,re,r,rp,c)
	local mi,ma=c:GetTributeRequirement()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft>0 and (ma==1 or (mi==1 and Duel.SelectYesNo(tp,aux.Stringid(18988391,2)))) then
	else
		local g=Duel.SelectTribute(tp,c,1,1)
		c:SetMaterial(g)
		Duel.Release(g,REASON_SUMMON+REASON_MATERIAL)
	end
	e:Reset()
end
