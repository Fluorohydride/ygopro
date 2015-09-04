--先史遺産ソル・モノリス
function c93543806.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(93543806,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c93543806.lvcost)
	e1:SetTarget(c93543806.lvtg)
	e1:SetOperation(c93543806.lvop)
	c:RegisterEffect(e1)
	Duel.AddCustomActivityCounter(93543806,ACTIVITY_SPSUMMON,c93543806.counterfilter)
end
function c93543806.counterfilter(c)
	return c:IsSetCard(0x70)
end
function c93543806.lvcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetCustomActivityCount(93543806,tp,ACTIVITY_SPSUMMON)==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c93543806.splimit)
	Duel.RegisterEffect(e1,tp)
end
function c93543806.splimit(e,c)
	return not c:IsSetCard(0x70)
end
function c93543806.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x70) and c:GetLevel()~=6
end
function c93543806.lvtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c93543806.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c93543806.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c93543806.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c93543806.lvop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LEVEL)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(6)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
	end
end
