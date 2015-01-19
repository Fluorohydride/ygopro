--TG サイバー・マジシャン
function c64910482.initial_effect(c)
	--synchro custom
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SYNCHRO_MATERIAL_CUSTOM)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetTarget(c64910482.syntg)
	e1:SetValue(1)
	e1:SetOperation(c64910482.synop)
	c:RegisterEffect(e1)
	--to grave
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetOperation(c64910482.regop)
	c:RegisterEffect(e2)
end
function c64910482.synfilter1(c,syncard,tuner,f)
	return c:IsFaceup() and c:IsNotTuner() and c:IsCanBeSynchroMaterial(syncard,tuner) and (f==nil or f(c))
end
function c64910482.synfilter2(c,syncard,tuner,f)
	return c:IsSetCard(0x27) and c:IsNotTuner() and c:IsCanBeSynchroMaterial(syncard,tuner) and (f==nil or f(c))
end
function c64910482.syntg(e,syncard,f,minc,maxc)
	local c=e:GetHandler()
	local lv=syncard:GetLevel()-c:GetLevel()
	if lv<=0 then return false end
	local g=Duel.GetMatchingGroup(c64910482.synfilter1,syncard:GetControler(),LOCATION_MZONE,LOCATION_MZONE,c,syncard,c,f)
	if syncard:IsSetCard(0x27) then
		local exg=Duel.GetMatchingGroup(c64910482.synfilter2,syncard:GetControler(),LOCATION_HAND,0,c,syncard,c,f)
		g:Merge(exg)
	end
	return g:CheckWithSumEqual(Card.GetSynchroLevel,lv,minc,maxc,syncard)
end
function c64910482.synop(e,tp,eg,ep,ev,re,r,rp,syncard,f,minc,maxc)
	local c=e:GetHandler()
	local lv=syncard:GetLevel()-c:GetLevel()
	local g=Duel.GetMatchingGroup(c64910482.synfilter1,syncard:GetControler(),LOCATION_MZONE,LOCATION_MZONE,c,syncard,c,f)
	if syncard:IsSetCard(0x27) then
		local exg=Duel.GetMatchingGroup(c64910482.synfilter2,syncard:GetControler(),LOCATION_HAND,0,c,syncard,c,f)
		g:Merge(exg)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local sg=g:SelectWithSumEqual(tp,Card.GetSynchroLevel,lv,minc,maxc,syncard)
	Duel.SetSynchroMaterial(sg)
end
function c64910482.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsPreviousLocation(LOCATION_ONFIELD) and c:IsReason(REASON_DESTROY) then
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(64910482,0))
		e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetRange(LOCATION_GRAVE)
		e1:SetTarget(c64910482.thtg)
		e1:SetOperation(c64910482.thop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
	end
end
function c64910482.filter(c)
	return c:IsSetCard(0x27) and c:GetCode()~=64910482 and c:IsAbleToHand()
end
function c64910482.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c64910482.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c64910482.thop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c64910482.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
